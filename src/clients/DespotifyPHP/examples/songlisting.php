<?php

/*

	Simple example using the DespotifyPHP interface to the Despotify gateway.
	This example lists all songs in a user's top positioned playlist.
	To run the example you must set the variables $username, $password, $gateway and $port at line 19-24
	
	Note that you must be running the Despotify gateway for this to work. Look for it in the SVN repository. It is located in /src/clients/gateway/

*/



include('../src/Despotify.php');


// replace these with a valid Spotify premium username and password
$username = 'your username';
$password = 'your password';

// specify how to connect to the gateway
$gateway = '127.0.0.1';
$port = 1234;


// create Despotify object which will be used for all interaction with the gateway(and Spotify, through the gateway)
$ds = new Despotify($gateway, $port);


// attempt to connect
if(!$ds->connect())
{
	die("Could not connect to gateway $gateway on port $port");
}


// attempt to login to Spotify
if(!$ds->login($username, $password))
{
	die('Could not authenticate. Wrong username or password?');
}


// get an array containing ids for all playlists this user has
$playlistArray = $ds->getPlaylistIds();

// iterate over the array of playlist ids
foreach($playlistArray as $playlistId)
{
	// get a Playlist object so that we can start extracting data
	$playlistObject = $ds->getPlaylist($playlistId);
	
	// print the playlist's name
	echo '<h1>' . $playlistObject->getName() . '</h1>';
	echo '<table><tr><td><b>Song name</b></td><td><b>Artist name</b></td></tr>';
	
	// get an array containing Track objects, representing the tracks in this playlist
	$trackArray = $playlistObject->getTracks();
	
	
	foreach($trackArray as $trackObject)
	{
		echo '<tr>';
			echo '<td><a href="' . $trackObject->getHTTPLink() . '">' . $trackObject->getName() . '</a></td>';
			echo '<td>';
			
			// artist name is returned as an array if there are multiple artists performing the song
			if(is_array($trackObject->getArtistName()))
			{
				// flatten the array
				echo implode(', ', $trackObject->getArtistName());
			}
			else
			{
				echo $trackObject->getArtistName();
			}
			
			echo '</td>';
		echo '</tr>';
	}
	echo '</table>';
	
	break; // in this example we break here, thus only songs in the first playlist are listed. Because otherwise it might take long time to load the page(with all playlists)
}



?>
