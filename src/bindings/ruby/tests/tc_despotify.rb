#!/usr/bin/env ruby

class TC_Despotify < Test::Unit::TestCase
	def test_uri
		assert_equal('2SfDMhAX5dmall6oGPGtp5', Despotify.id2uri('5e76f4a1832b42708fd0793c92585a27'))
		assert_equal('5e76f4a1832b42708fd0793c92585a27', Despotify.uri2id('2SfDMhAX5dmall6oGPGtp5'))

		assert_equal('2sDfbgbPqK8cnEDV5bCyby', Despotify.id2uri('50e342e4cd454196b9eae64cd7bfa2f4'))
		assert_equal('50e342e4cd454196b9eae64cd7bfa2f4', Despotify.uri2id('2sDfbgbPqK8cnEDV5bCyby'))

		assert_equal('6EUwMKcf7weEP4JsLmKJKR', Despotify.id2uri('dad2818b7ec2a12d3e76d50f3cdcbd9d'))
		assert_equal('dad2818b7ec2a12d3e76d50f3cdcbd9d', Despotify.uri2id('6EUwMKcf7weEP4JsLmKJKR'))

		assert_equal('26b0ZNVmaWQWuKAEchHog5', Despotify.id2uri('44fcd13c9ddc40d382a27310bc95057d'))
		assert_equal('44fcd13c9ddc40d382a27310bc95057d', Despotify.uri2id('26b0ZNVmaWQWuKAEchHog5'))
	end
end
