#!/usr/bin/env ruby

require 'test/unit/testsuite'
require 'test/unit/ui/console/testrunner'

$LOAD_PATH << '../build'

require 'despotify'

if ARGV.length < 2
	puts 'Need username & password'
	exit
end

USERNAME = ARGV.shift
PASSWORD = ARGV.shift

SESSION = Despotify::Session.new
SESSION.authenticate(USERNAME, PASSWORD)


require 'tc_albumbrowse'
require 'tc_artistbrowse'
require 'tc_despotify'
require 'tc_playlist'
require 'tc_search'
require 'tc_session'
require 'tc_track'


class TS_Despotify
	def self.suite
		suite = Test::Unit::TestSuite.new('Despotify Ruby')
		suite << TC_Despotify.suite
		suite << TC_Session.suite
		suite << TC_AlbumBrowse.suite
		suite << TC_ArtistBrowse.suite
		suite << TC_Playlist.suite
		suite << TC_Search.suite
		suite << TC_Track.suite

		return suite
	end
end

Test::Unit::UI::Console::TestRunner.run(TS_Despotify, output_level=Test::Unit::UI::VERBOSE)
