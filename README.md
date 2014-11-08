Code Golf - Rearrange the pixels

This is my implementation of the following challange:

http://codegolf.stackexchange.com/questions/33172/american-gothic-in-the-palette-of-mona-lisa-rearrange-the-pixels

I've implemented a "reference" implementation on that link with the idea in the top scored solution (Java - GUI with progressive randomized transformation) that I used to compare my results.

My algorithms are composition of "technique" and "distance" algorithms, that means all color comparision are done ba some kind of distance formulae. The technique is how do we iterate to compare the distance.

As for distance, I've implemented four algorithms:

- Color Metric (http://www.compuphase.com/cmetric.htm)
- CieDe 2000 (https://github.com/THEjoezack/ColorMine/blob/master/ColorMine/ColorSpaces/Comparisons/CieDe2000Comparison.cs)
- Cie 1967
- HSV Hue Compare

As for technique, there are three simple implementations:

- Random Pixel Swap (reference - best score on stack exchange)
	- The idea is just to pick two random pixels on both image, compare them and swap them only if they are closer.
- Indexed Replace
	- Sort all pixels on both images, then replace from first image on the second based on the array index only.
- Bisect
	- Single: Sort all pixels on both images, find using bisect the closer one on the second image for each pixel in the first one, consuming both.
	- Threaded: Same, but we do with subsections of the image, this cause banding on the image depending on the pixel color quantity and distribution.
		- The results are very nice and is fast.

This was a nice exercice to remember Qt and do some C++11 coding, the challange was just an excuse anyway ;)
