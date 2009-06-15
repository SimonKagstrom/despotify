#!/usr/bin/env ruby

class TC_Playlist < Test::Unit::TestCase
	def setup
		@playlist = Despotify::Playlist.new(SESSION, 'dad2818b7ec2a12d3e76d50f3cdcbd9d02')
		@splaylist = SESSION.stored_playlists.first
	end


	def test_metadata
		assert_kind_of(Despotify::Playlist, @playlist)
		assert_kind_of(String, @playlist.name)
		assert_kind_of(String, @playlist.id)
		assert_kind_of(String, @playlist.author)
		assert_kind_of(Array, @playlist.tracks)
		assert_kind_of(Despotify::Track, @playlist.tracks.first)

		assert_equal('dad2818b7ec2a12d3e76d50f3cdcbd9d02', @playlist.id)
		assert_equal('spock', @playlist.name)
		assert_equal('chripppa', @playlist.author)
	end

	def test_modify
		origname = @splaylist.name

		@splaylist.rename('test rename')
		assert_equal('test rename', @splaylist.name)

		@splaylist.rename(origname)
		assert_equal(origname, @splaylist.name)
	end

	def test_uri
		assert_kind_of(String, @playlist.to_uri)
		assert_equal('spotify:user:chripppa:playlist:6EUwMKcf7weEP4JsLmKJKR', @playlist.to_uri)
	end
end
