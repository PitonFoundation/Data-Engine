CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * Dependencies
 * Layouts
 * Fields


INTRODUCTION
------------

DS html5 is an add on for Display Suite that adds semantic layouts and field
templates.


INSTALLATION
------------

Download the module as you would any other and enable it.  For field templates,
you must also enable the ds_extras module, which is bundled with Display Suite
and check "Enable field templates" at /admin/structure/ds/extras.


DEPENDENCIES
------------

Display Suite
ds_extras (Only required for field templates.)


LAYOUTS
-------

Article - Article tag including regions for content ( a div ), header, hgroup,
aside, figure and footer.

Figure - Figure tag including regions for figure (no extra wrapping tag) and
figcaption.

No Markup - A single region for content that renders no wrapping markup,
instead it just renders fields directly.

Section - Section tag including regions for content ( a div ), header, hgroup,
aside, figure and footer.


FIELDS
------

These fields replace the default wrapper divs on fields with semantic tags.
In order to use the DS HTML5 fields, you must enable the ds_extras module and
turn on "Enable custom field templates" at /admin/structure/ds/extras.

Included Field Templates
h1
h2
h3
h4
h5
h6
dl
ol
ul
address
article
aside
header
figure
footer
nav
section
span
