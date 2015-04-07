#!/usr/bin/perl
# Fades from red to blue to green, with each RGB value
# written by a different thread.
$red = 2047;
$green = 0;
$blue = 0;
$SIG{CHLD} = 'IGNORE';
while ($red > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		system("./client $red $green $blue");
		exit 0;
	}
	$red--;
	$green++;
}
while ($green > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		system("./client $red $green $blue");
		exit 0;
	}
	$green--;
	$blue++;
}
while ($green > 0) {
	$pid = fork();
	if (defined $pid && $pid == 0) {
		system("./client $red $green $blue");
		exit 0;
	}
	$blue--;
	$red++;
}
