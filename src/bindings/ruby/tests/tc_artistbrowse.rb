#!/usr/bin/env ruby

class TC_ArtistBrowse < Test::Unit::TestCase
	def setup
		@artist = Despotify::ArtistBrowse.new(SESSION, '50e342e4cd454196b9eae64cd7bfa2f4')
	end

	def test_metadata
		assert_kind_of(Despotify::ArtistBrowse, @artist)
		assert_kind_of(String, @artist.name)
		assert_kind_of(String, @artist.id)
		assert_kind_of(Hash, @artist.metadata)
		assert_kind_of(String, @artist['id'])
		assert_kind_of(String, @artist['name'])
		assert_kind_of(String, @artist['portrait_id'])
		assert_kind_of(String, @artist['years_active'])
		assert_kind_of(String, @artist['genres'])
		assert_kind_of(Fixnum, @artist['num_albums'])
		assert_kind_of(Float, @artist['popularity'])
		assert_kind_of(Despotify::AlbumBrowse, @artist.albums.first)

		assert_equal('50e342e4cd454196b9eae64cd7bfa2f4', @artist.id)
		assert_equal('50e342e4cd454196b9eae64cd7bfa2f4', @artist['id'])
		assert_equal('S.P.O.C.K', @artist.name)
		assert_equal('S.P.O.C.K', @artist['name'])
		assert_equal(@artist.albums.length, @artist['num_albums'])
		assert_equal('1988-', @artist['years_active'])
		assert_equal('Synth Pop,Rock,New Wave,Alternative Pop/Rock', @artist['genres'])
	end

	def test_uri
		assert_kind_of(String, @artist.to_uri)
		assert_equal('spotify:artist:2sDfbgbPqK8cnEDV5bCyby', @artist.to_uri)
	end
end
