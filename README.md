Rainbow
=======

<p align=”center”>
  <img src="https://github.com/dinosaure/Rainbow/blob/master/logo.gif?raw=true" alt="Rainbow" />
</p>

Ceci est une implémentation imparfaite des arbres rouge et noir en C avec OCaml. Je voulais voir si une implémentation en C
serait plus rapide qu’en OCaml mais je sais pas parce que j’ai la flemme des faire les _benchmark_.
Ensuite, elle est imparfaite parce que pour le module `Poney`, il manque le `remove` (pour ceux qui ont déjà implémenté
un arbre rouge et noir savent de quoi je parle) car il faudrait rester en phase avec _garbage collector_.

Donc si vous avez des idées pour sauver le petit `Poney`, je suis à l’écoute.

Enfin, il y a le module `Unicorn`. Il marche presque, on peut notifier un bug dans le ̀`main.ml` en rajoutantt
`Gc.compact ()`. Donc pour ceux qui sont curieux, voilà ma superbe implémentation. Bien entendu, l’image est en lien
avec l’horreur que vous pouvez voir dans ce code.
