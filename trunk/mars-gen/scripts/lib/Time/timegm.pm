#  You may distribute under the terms of either the GNU General Public License
#  or the Artistic License (the same terms as Perl itself)
#
#  (C) Paul Evans, 2012 -- leonerd@leonerd.org.uk

package Time::timegm;

use strict;
use warnings;

our $VERSION = '0.01';

use Exporter 'import';

our @EXPORT_OK = qw(
   timegm
);

unless( eval { 
   require XSLoader;
   XSLoader::load( __PACKAGE__, $VERSION );
   defined Time::timegm::timegm( 0, 0, 0, 0, 1, 70 )
} ) {
   # Fallback on pureperl implementation
   require POSIX;
   no warnings 'redefine';
   *timegm = \&timegm_pp;
}

use Carp;

use POSIX qw( mktime );

# Number of seconds in a POSIX day
use constant DAY  => 24*60*60;
use constant HOUR => 60*60;
use constant MIN  => 60;

=head1 NAME

C<Time::timegm> - a UTC version of C<mktime()>

=head1 SYNOPSIS

 use Time::timegm qw( timegm );

 my $epoch = timegm( 0, 0, 0, 14, 6-1, 2012-1900 );

 print "2012-06-14 00:00:00 UTC happened at ",
    scalar localtime($epoch), " localtime\n";

=head1 DESCRIPTION

The L<POSIX> standard provides three functions for converting between integer
epoch values and 6-component "broken-down" time representations. C<localtime>
and C<gmtime> convert an epoch into the 6 components of seconds, minutes,
hours, day of month, month and year, in either local timezone or UTC. The
C<mktime> function converts a local broken-down time into an epoch value.
However, C<POSIX> does not provide a UTC version of this.

This module provides a function C<timegm> which has this ability.

Unlike some other CPAN implementations of this behaviour, this version does
not re-implement the time handling logic internally. It reuses the C<mktime>
and C<gmtime> functions provided by the system to ensure its results are
always consistent with the other functions.

=cut

=head1 FUNCTIONS

=cut

=head2 $epoch = timegm( $sec, $min, $hour, $mday, $mon, $year )

Returns the epoch integer value representing the time given by the 6
broken-down components.

As with C<POSIX::mktime> it is I<not> required that these values be within
their "valid" ranges. This function will normalise values out of range. For
example, the 25th hour of a day is normalised to the 1st hour of the following
day; or the 0th month is normalised to the 12th month of the preceeding year.

=cut

# Cache "$year/$mon" => $epoch of the time that month begins
my %start_of_month;

sub timegm_pp
{
   my ( $sec, $min, $hour, $mday, $mon, $year ) = @_;

   # Epoch times + UTC always align day boundaries at multiples of 86400. The
   # structure is mathematically regular within a month. Therefore we can
   # calculate the epoch time for a given time in UTC by finding the start of
   # the month then adding seconds

   $mon-=12, $year++ while $mon >= 12;
   $mon+=12, $year-- while $mon < 0;

   my $monstart = $start_of_month{"$year/$mon"};
   unless( defined $monstart ) {
      $monstart = mktime( 0, 0, 0, 1, $mon, $year );
      $monstart -= $monstart % DAY;

      my @gmtime = gmtime $monstart;
      $gmtime[$_] == 0 or croak "Expected midnight GMT, did not get it - " . scalar( gmtime $monstart ) for 0 .. 2;

      # Might have to round forward rather than backward 
      $monstart += DAY, @gmtime = gmtime $monstart if $gmtime[3] > 1;

      $gmtime[3] == 1 or croak "Expected 1st of month GMT, did not get it - " . scalar( gmtime $monstart );

      $start_of_month{"$year/$mon"} = $monstart;
   }

   return $monstart + ($mday-1)*DAY + $hour*HOUR + $min*MIN + $sec;
}

=head1 COMPARISON WITH Time::Local

The L<Time::Local> module also provides a function called C<timegm()> with
similar behaviour to this one. The differences are:

=over 2

=item *

C<Time::timegm::timegm()> handles denormalised values (that is, seconds or
minutes outside of the range 0 to 59, hours outside 0 to 23, etc..) by
adjusting the next largest unit (such that 61 seconds is 1 second of the next
minute, etc). C<Time::Local::timegm()> croaks on out-of-range input.
C<Time::Local> also provides a function C<timegm_nocheck()> which does not
croak but it is documented that the behavior is unspecified on out-of-range
values.

=item *

C<Time::timegm::timegm()> is implemented by a light XS wrapper around the
C<timegm(3)> or C<_mkgmtime(3)> function provided by the platform's C library
if such a function is provided, so its behaviour is consistent with the rest
of the platform. C<Time::Local> re-implements the logic in perl code.
C<Time::timegm> will fall back to a perl implementation only if the XS one
cannot be used.

=back

=head1 AUTHOR

Paul Evans <leonerd@leonerd.org.uk>

=cut

0x55AA;
