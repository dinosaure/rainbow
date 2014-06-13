RainBow
=======

<p align=”center”>
  <img src="https://github.com/dinosaure/Rainbow/blob/master/logo.gif?raw=true" alt="Rainbow" />
</p>

This is an imperfect implementation of red-black tree in C with OCaml. I wanted to see if a
C implementation would be faster than OCaml but I don’t know because I’m too lazy to do the
_benchmark_. Then it is imperfect because for `Poney` module, it misses the `remove` (for
those who have already implemented a red-black tree know what I mean) because it would keep
up with the _garbage collector_.

So if you have ideas to save the little `Poney`, I’m listening.

Finally, there is the `Unicorn` module. It almost works, we can notify a bug in `main.ml` by
adding `Gc.compact ()`. So for those who are curious, here is my super implementation. Of course,
the image is linked to the horror that you can see in this code.
