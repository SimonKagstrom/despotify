<?php

// define constans
define('LOAD_FROM_XML', 0);
define('LOAD_FROM_ID', 1);

// do you want to run in debug mode? setting this to true will activate debug message output
define('DEBUG_MODE', false);



function debug_msg($message)
{
	echo '#DEBUG OUTPUT: <b>' . $message . '</b><br/>';
}


function toId($number)
{
	$decimal = hexdec($number);
	return dec2any($decimal);
}


function chopDespotifyId($despotifyId)
{
	return substr($despotifyId, 0, 32);
}


function toDespotifyId($spotifyId)
{
	return str_pad(s_base_convert($spotifyId, 62, 16), 32, '0', STR_PAD_LEFT);
}


function toSpotifyId($despotifyId)
{
	return str_pad(s_base_convert($despotifyId, 16,62), 22, '0', STR_PAD_LEFT);
}


function isValidDespotifyId($id)
{
	if(strlen($id) == 32)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*function isValidSpotifyId($id)
{
	return true;
}*/


// borrowed from Despotify's PHP class
function s_base_convert ($numstring, $frombase, $tobase) {
	$chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	$tostring = substr($chars, 0, $tobase);

	$length = strlen($numstring);
	$result = '';
	for ($i = 0; $i < $length; $i++) {
			$number[$i] = strpos($chars, $numstring{$i});
	}
	do {
			$divide = 0;
			$newlen = 0;
			for ($i = 0; $i < $length; $i++) {
				$divide = $divide * $frombase + $number[$i];
				if ($divide >= $tobase) {
						$number[$newlen++] = (int)($divide / $tobase);
						$divide = $divide % $tobase;
				} elseif ($newlen > 0) {
						$number[$newlen++] = 0;
				}
			}
			$length = $newlen;
			$result = $tostring{$divide} . $result;
	}
	while ($newlen != 0);
	return $result;
}


?>