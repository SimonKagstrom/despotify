#!/usr/bin/env ruby

$LOAD_PATH << '../build'

require 'despotify'
require 'pp'


class Simple < Despotify::Session
	def initialize(username, password)
		puts 'init'
		super(username, password)
	end
end

class SuperPlaylist < Despotify::Playlist
	def dump
		tracks.each do |track|
			pp track.metadata
		end
	end
end



username = ARGV.shift
password = ARGV.shift

if not (username and password)
	puts 'Need username & password'
	exit
end

c = Simple.new(username, password)

pls = SuperPlaylist.new(c, 'd10d32140807f5260d045384117734c002')
pls.dump
