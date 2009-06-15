#!/usr/bin/env ruby

class TC_Session < Test::Unit::TestCase
	def test_userinfo
		info = SESSION.user_info

		assert_kind_of(String, info['username'])
		assert_equal(USERNAME, info['username'])
		assert_kind_of(String, info['country'])
		assert_kind_of(Fixnum, info['expiry'])
		assert_kind_of(Fixnum, info['last_ping'])
		assert_kind_of(String, info['server_host'])
		assert_kind_of(Fixnum, info['server_port'])
		assert_kind_of(String, info['type'])
	end

	def test_shortcuts
		assert_kind_of(Despotify::AlbumBrowse, SESSION.album('5e76f4a1832b42708fd0793c92585a27'))
		assert_kind_of(Despotify::ArtistBrowse, SESSION.artist('50e342e4cd454196b9eae64cd7bfa2f4'))
		assert_kind_of(Despotify::Playlist, SESSION.playlist('dad2818b7ec2a12d3e76d50f3cdcbd9d02'))
		assert_kind_of(Despotify::Search, SESSION.search('britney spears', 50))
		assert_kind_of(Despotify::Track, SESSION.track('44fcd13c9ddc40d382a27310bc95057d'))
	end

	def test_get_image
		data = SESSION.get_image('823248cb726945a9a8528c1485ed484b1a672557')

		assert_kind_of(String, data)
		assert_equal(47441, data.length)
	end

	def test_playlists
		assert_kind_of(Despotify::Playlist, SESSION.stored_playlists[0])
	end

	def test_uri
		assert_kind_of(Despotify::AlbumBrowse, SESSION.uri('spotify:album:2SfDMhAX5dmall6oGPGtp5'))
		assert_kind_of(Despotify::ArtistBrowse, SESSION.uri('spotify:artist:2sDfbgbPqK8cnEDV5bCyby'))
		assert_kind_of(Despotify::Playlist, SESSION.uri('spotify:user:chripppa:playlist:6EUwMKcf7weEP4JsLmKJKR'))
		assert_kind_of(Despotify::Search, SESSION.uri('spotify:search:britney spears'))
		assert_kind_of(Despotify::Track, SESSION.uri('spotify:track:26b0ZNVmaWQWuKAEchHog5'))
		assert_nil(SESSION.uri('spotify:invalid uri'))
	end
end
