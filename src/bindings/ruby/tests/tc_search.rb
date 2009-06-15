#!/usr/bin/env ruby

class TC_Search < Test::Unit::TestCase
	def setup
		@search = Despotify::Search.new(SESSION, 'britney spears', 50)
	end

	def test_metadata
		assert_kind_of(Despotify::Search, @search)
		assert_kind_of(Hash, @search.metadata)
		assert_kind_of(String, @search['query'])
		assert_kind_of(String, @search['suggestion'])
		assert_kind_of(Fixnum, @search['total_albums'])
		assert_kind_of(Fixnum, @search['total_artists'])
		assert_kind_of(Fixnum, @search['total_tracks'])
		assert_kind_of(Despotify::Album, @search.albums.first)
		assert_kind_of(Despotify::Artist, @search.artists.first)
		assert_kind_of(Despotify::Playlist, @search.playlist)
		assert_kind_of(Despotify::Track, @search.tracks.first)

		assert_equal('britney spears', @search['query'])
		assert_equal(50, @search.tracks.length)
	end

	def test_append
		@search.append(@search.tracks.length, 50)
		assert_equal(100, @search.tracks.length)
	end

	def test_uri
		assert_kind_of(String, @search.to_uri)
		assert_equal('spotify:search:britney spears', @search.to_uri)
	end
end
