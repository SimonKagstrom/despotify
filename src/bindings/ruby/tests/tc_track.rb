#!/usr/bin/env ruby

class TC_Track < Test::Unit::TestCase
	def setup
		@track = Despotify::Track.new(SESSION, '44fcd13c9ddc40d382a27310bc95057d')
	end

	def test_metadata

		assert_kind_of(Despotify::Track, @track)
		assert_kind_of(Hash, @track.metadata)
		assert_kind_of(Despotify::Artist, @track.artists.first)
		assert_kind_of(String, @track.id)
		assert_kind_of(String, @track['id'])
		assert_kind_of(String, @track['album'])
		assert_kind_of(String, @track['title'])
		assert_kind_of(String, @track['file_id'])
		assert_kind_of(String, @track['album_id'])
		assert_kind_of(String, @track['cover_id'])
		assert_kind_of(TrueClass, @track['playable'])
		assert_kind_of(Fixnum, @track['length'])
		assert_kind_of(Fixnum, @track['tracknumber'])
		assert_kind_of(Fixnum, @track['year'])
		assert_kind_of(Float, @track['popularity'])

		assert_equal('44fcd13c9ddc40d382a27310bc95057d', @track.id)
		assert_equal('44fcd13c9ddc40d382a27310bc95057d', @track['id'])
		assert_equal('S.P.O.C.K', @track.artists.first.name)
		assert_equal('2001: A S.P.O.C.K Odyssey', @track['album'])
		assert_equal('Astrogirl\'s Secret', @track['title'])
		assert_equal('a1eb5d51334c7a2a0d645ebd2df9d8d9bfa10b86', @track['file_id'])
		assert_equal('5e76f4a1832b42708fd0793c92585a27', @track['album_id'])
		assert_equal('823248cb726945a9a8528c1485ed484b1a672557', @track['cover_id'])
		assert_equal(true, @track['playable'])
		assert_equal(252573, @track['length'])
		assert_equal(10, @track['tracknumber'])
		assert_equal(2001, @track['year'])
	end

	def test_uri
		assert_kind_of(String, @track.to_uri)
		assert_equal('spotify:track:26b0ZNVmaWQWuKAEchHog5', @track.to_uri)
	end
end
