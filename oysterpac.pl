#!/usr/bin/perl

use strict;
use warnings;
use Fcntl qw(:seek);

print "oysterpac 0.1 - TI program variable packer by utz\n";

my $debuglvl = $#ARGV + 1;

if ($debuglvl != 2) {
	print "syntax: oysterpac.pl <infile> <outfile>\n";
	exit 1;
}

my $infile = $ARGV[0];
my $outfile = $ARGV[1];
my $target = substr $ARGV[1], -3;

if ($target ne '82p' && $target ne '83p' && $target ne '8xp' && $target ne '85s' && $target ne '86p' && $target ne '73p') {
	print "ERROR: Invalid target file type.\n";
	exit 1;
}

#check if binfile is present, and open it if it is
if ( -e $infile ) {
	open INFILE, $infile or die "ERROR: Could not open $infile: $!";
	binmode INFILE;
	print "Converting to $target...\n";
} 
else {
	print "ERROR: $infile not found.\n";
	exit 1;
}

my $binsize = -s $infile;
my $datasize = $binsize + 20;
my $varsize = $binsize + 5;
my $shortvarsize = $varsize - 2;
my $buffer;

#generate on-calculator name
my $oncalcname = substr $ARGV[1], 0, -4;
$oncalcname = uc($oncalcname);		#convert to uppercase
$oncalcname = pack('A7', $oncalcname) if ($target eq '82p');	#truncate or append to 7 chars
$oncalcname = pack('A8', $oncalcname) if ($target eq '83p');
print "$oncalcname check\n";
#print "$binsize\n";
open OUTFILE, ">$outfile" or die $!;
binmode OUTFILE;

if ($target eq '82p') {
	#write header
	print OUTFILE "**TI82**", pack('c3',26,10,0), "packed with oysterpac", pack('c21',0);
	print OUTFILE pack('s<',$datasize), pack('c2',11,0), pack('s<',$varsize);
	print OUTFILE pack('s<',56326);
	print OUTFILE "$oncalcname"; #, pack('c2',0,45);		#test - after the name it works like this: one 0 - if not 7 bytes, ascii minus, then # of bytes missing
	print OUTFILE pack('s<',$varsize), pack('s',$shortvarsize);
	print OUTFILE pack('s<',213),pack('c1',17);

	#copy binfile
	while (
		read (INFILE, $buffer, $binsize)
		and print OUTFILE $buffer
	){};
	die "ERROR: Problem copying: $!\n" if $!;

	#calculate checksum
	my $checksum = 0;
	my ($byteval, $i);

	close OUTFILE;
	open INFILE2, $outfile;
	binmode INFILE2;
	my $checksize = -s $outfile;

	for ($i = 60; $i < $checksize; $i++) {
		sysseek(INFILE2, $i, 0) or die $!;
		sysread(INFILE2, $byteval, 1) == 1 or die $!;
		$checksum = $checksum + ord($byteval);		
	}
	close INFILE2;
	open OUTFILE, ">>$outfile" or die $!;
	binmode OUTFILE;
	$checksum = $checksum - 86;
	print OUTFILE pack('s<',$checksum);
}

if ($target eq '83p') {
	#write header
	print OUTFILE "**TI83**", pack('c3',26,10,0), "packed with oysterpac", pack('c21',0);
	print OUTFILE pack('s<',$datasize), pack('c2',11,0), pack('s<',$varsize);
	#print OUTFILE pack('s<',56326);	#0xdc06
	print OUTFILE pack('c1',6);
	print OUTFILE "$oncalcname"; #, pack('c2',0,45);		#test - after the name it works like this: one 0 - if not 7 bytes, ascii minus, then # of bytes missing
	print OUTFILE pack('s<',$varsize), pack('s',$shortvarsize);
	print OUTFILE pack('s<',213),pack('c1',17);

	#copy binfile
	while (
		read (INFILE, $buffer, $binsize)
		and print OUTFILE $buffer
	){};
	die "ERROR: Problem copying: $!\n" if $!;

	#calculate checksum
	my $checksum = 0;
	my ($byteval, $i);

	close OUTFILE;
	open INFILE2, $outfile;
	binmode INFILE2;
	my $checksize = -s $outfile;

	for ($i = 60; $i < $checksize; $i++) {
		sysseek(INFILE2, $i, 0) or die $!;
		sysread(INFILE2, $byteval, 1) == 1 or die $!;
		$checksum = $checksum + ord($byteval);		
	}
	close INFILE2;
	open OUTFILE, ">>$outfile" or die $!;
	binmode OUTFILE;
	$checksum = $checksum - 86;
	print OUTFILE pack('s<',$checksum);
}

close INFILE;
close OUTFILE;