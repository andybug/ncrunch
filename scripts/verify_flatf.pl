#!/usr/bin/perl

use Term::ANSIColor;


if ($#ARGV < 0) {
	print "usage: call with path to a flatf to check\n";
	exit -1;
}

$filename = $ARGV[0];
$line_num = 1;

open FLATF, '<', $filename or die $!;


print "Reading field headings...\t\t\t";

@field_names = split(/\t+/, <FLATF>);
foreach $field (@field_names) {
	$field =~ s/[\t\n]+//;

	if (!&isAlpha($field)) {
		&printFAILED;
		print "field heading '$field' is not valid\n";
		exit -2;
	}
}
&printOK;


%field_types = ();
while (my $line = <FLATF>) {
	$line_num++;
	my @fields = split(/\t+/, $line);
	my $field_num = 0;

	print "Reading team on line $line_num...\t\t\t";

	foreach $field (@fields) {
		$field =~ s/[\t\n]//;

		if (&isAlpha($field)) {
			if (!defined $field_types{ $field_names[$field_num] }) {
				$field_types{ $field_names[$field_num] } = 'alpha';
			}

			elsif ($field_types{ $field_names[$field_num] } ne 'alpha') {
				&printFAILED;
				print "expected numeric for field '$field_names[$field_num]' on line $line_num; got '$field' instead\n";
				exit -3;
			}
		}

		elsif (&isNumeric($field)) {
			if (!defined $field_types{ $field_names[$field_num] }) {
				$field_types{ $field_names[$field_num] } = 'numeric';
			}

			elsif ($field_types{ $field_names[$field_num] } ne 'numeric') {
				&printFAILED;
				print "expected alpha for field '$field_names[$field_num]' on line $line_num; got '$field' instead\n";
				exit -3;
			}
		}

		else {
			&printFAILED;
			print "invalid field data '$field' on line $line_num\n";
			exit -3;
		}

		$field_num++;
	}

	&printOK;
}

close FLATF;
print "\t\t\t\t\t\t";
&printDONE;



sub isAlpha {
	return $_[0] =~ /^[a-zA-Z &]+$/;
}


sub isNumeric {
	return $_[0] =~ /^[0-9]+$/;
}


sub printOK {
	print '[  ';
	print color 'green';
	print 'OK';
	print color 'reset';
	print '  ]';
	print "\n";
}


sub printFAILED {
	print '[';
	print color 'red';
	print 'FAILED';
	print color 'reset';
	print ']';
	print "\n";
}


sub printDONE {
	print '[ ';
	print color 'green';
	print 'DONE';
	print color 'reset';
	print ' ]';
	print "\n";
}

