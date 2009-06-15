#!/usr/bin/env ruby

class TC_AlbumBrowse < Test::Unit::TestCase
	def setup
		@album = Despotify::AlbumBrowse.new(SESSION, '5e76f4a1832b42708fd0793c92585a27')
	end

	def test_metadata
		assert_kind_of(Despotify::AlbumBrowse, @album)
		assert_kind_of(String, @album.name)
		assert_kind_of(String, @album.id)
		assert_kind_of(Hash, @album.metadata)
		assert_kind_of(Array, @album.tracks)
		assert_kind_of(Despotify::Track, @album.tracks.first)
		assert_kind_of(String, @album['id'])
		assert_kind_of(String, @album['name'])
		assert_kind_of(String, @album['cover_id'])
		assert_kind_of(Float, @album['popularity'])
		assert_kind_of(Fixnum, @album['num_tracks'])
		assert_kind_of(Fixnum, @album['year'])

		assert_equal('5e76f4a1832b42708fd0793c92585a27', @album.id)
		assert_equal('5e76f4a1832b42708fd0793c92585a27', @album['id'])
		assert_equal('2001: A S.P.O.C.K Odyssey', @album.name)
		assert_equal('2001: A S.P.O.C.K Odyssey', @album['name'])
		assert_equal(13, @album.tracks.length)
		assert_equal(13, @album['num_tracks'])
		assert_equal('823248cb726945a9a8528c1485ed484b1a672557', @album['cover_id'])
		assert_equal(2001, @album['year'])
	end

	def test_uri
		assert_kind_of(String, @album.to_uri)
		assert_equal('spotify:album:2SfDMhAX5dmall6oGPGtp5', @album.to_uri)
	end
end
