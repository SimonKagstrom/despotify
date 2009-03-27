#!/usr/bin/env ruby

$LOAD_PATH << '../build'

require 'despotify'
require 'pp'

username = ARGV.shift
password = ARGV.shift

if not (username and password)
	puts 'Need username & password'
	exit
end

begin
	despotify = Despotify::Session.new(username, password)
rescue Despotify::DespotifyError
	puts 'Failed to authenticate user'
	exit
end

despotify.playlists.each do |pls|
	pls.tracks.each do |track|
		pp track.metadata
	end
end
