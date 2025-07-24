<p align="center">
<img src="https://rares-cosma.github.io/logo.png" alt="Logo" width="200" height="200">
</p>

<h1 align="center">DOCUMENTAȚIE</h1>
<h2 align="center">ROScript</h2>

## ❓ Descrierea problemei

Ca iubitor de informatică, am descoperit două probleme în predarea informaticii în licee, parcurgându-mi primul an de liceu. Din discuțiile cu profesori, olimpici și elevi cu diferite pasiuni, am observat că cele 2 probleme fundamentale în înțelegerea informaticii sunt **barierele de limbaj** și **rigiditatea pseudocodului**.

Problema principală este că pseudocodul nu poate fi *executat*. Majoritatea elevilor ajung să parcurgă manual diferite bucăți de pseudocod, fara a întelege utilitatea acestuia, sau a informaticii. Acesta de altfel poate fi găsit în diferite forme, neavând un standard. Când aceștia încep să studieze C++, un limbaj complex, nu au bazele necesare, iar ca urmare nu înteleg informatica.

## 💾 Descrierea soluției

În urma deficiențelor identificate, am decis să dezvolt ROScript, primul limbaj independent în limba română. Acesta poate executa codul asemănator cu pseudocodul folosind un interpretor, facilitând astfel trecerea mai lină de la teorie la practică în C++.

Acesta prezintă un interpretor cu arhitectura Tree-Walk, funcționalități moderne și portabilitate pe majoritatea platformelor. 

## 👥 Publicul țintă

ROScript tinde sa fie un limbaj cât mai accesibil pentru oricine, dar a fost conceput cu acest public țintă:
* Elevii de liceu din clasele IX–XII, în special de la profilurile Matematică-Informatică și Științe ale Naturii
* Persoane care nu cunosc limba engleză, dar vor să poată învăța bazele programării în limba lor maternă
* Începătorilor în programare, care își doresc o introducere ușor de înțeles în conceptele de bază

## 🧑‍🏫 Propunere pentru introducerea în sistemul de învățământ

Pentru valorificarea potențialului limbajului propun introducerea ROScript în programa oficială de informatică, după cum urmează:

### 1. În ciclul gimnazial

Elevii de gimnaziu ar putea folosi ROScript, pentru un modul, în vederea învățării algoritmicii de bază.

### 2. În liceu, în paralel cu pseudocodul clasic

Consider că predarea clasică a pseudocodului are avantajele sale, dar ar putea fi suplimentată de ROScript. Elevii ar beneficia de logica pseudocodului clasic, în timp ce ar avea posibilitatea să execute și să faca debug codului.

### 3. În liceu, ca introducere în Inteligența Artificială

Inteligența artificială este incontestabil un domeniu de viitor, iar ROScript are capacitățile necesare unei introduceri în AI. Listele dinamice conferă o flexibilitate și o ușurință necesară în predarea inteligenței artificiale. Pentru un exemplu real, se poate găsi un exemplu de regresie liniară în ```examples/linear_regression.ros```

## ⚙️ Funcționalități

### Variabile - static și dynamic typing - declarare cu var sau cu type specifier (intreg, logic, sirc, real)

```var x = 12*2;``` - declarare dinamică  
```intreg y = 23;``` - declarare statică  
```real pi = 3.1415;```

### Comentarii

```// Acesta este un comentariu de o linie```

```
/* Acesta este un comentariu
multi-line */
```

### Control Flow - structuri condiționale și repetitive (pentru, repeta pana cand, repeta cat timp, cat timp)

1. If - Dacă - structură condițională
```
daca (conditie) atunci {
    // instructiuni
} altfel daca (conditie) {
    // alte instructiuni
} altfel {
    // instructiuni
}
```

2. While - Cât timp - structură repetitivă

```
cat timp (conditie) executa {
    // instructiuni
}
```

3. Do while/until - Repetă până când/cât timp - structuri repetitive

```
repeta {
    // instructiuni
} pana cand (conditie);

repeta {
    // instructiuni
} cat timp (conditie);
```

4. For - Pentru - structură repetitivă

```
pentru (declarare/atribuire; conditie; atribuire) executa {
    // instructiuni
}
```

### Biblioteca Standard - I/O, matematică, liste

```
var x = radp(2); // radacina patrata de 2
x = sirc(x); // transformam x in sir de caractere
afiseaza(x, tip(x)); // afisam x si tipul sau
```

### Liste - liste rapide pe n dimensiuni, neomogene și dinamice

```
var x = [1,3.4,"ROScript"]; // lista neomogena
adauga(x,fals); 

var y = [
    [1,2,3],
    [4,5,6],
    [7,8,9]
]; // matrice

var z = [[[1]]];
afiseaza(z[0,0,0]); // => 1
```

### Funcții - funcții user-defined cu recursivitate și scoping

```
functie adauga(var x, var y) {
    var z = x+y;
    returneaza z;
}

afiseaza(adauga(2,3)); // => 5
```

## Instalare

Sunt 2 metode principale pentru instalarea interpretorului:

**Prin descărcarea unui executabil precompilat**

Se va descărca executabilul aferent fiecărui sistem de operare din folderul /interpreter/bin/.

Windows: /interpreter/bin/ros.exe

Linux: /interpreter/bin/ros

**Prin compilarea sursei**

Toate fișierele sursă se află în folderul /interpreter/src, cu excepția fișierului pentru automatizarea testelor. Aceste surse pot fi compilate executând fisierul ```compile.sh``` din folderul /interpreter/bin. Acesta poate fi rulat nativ pe Linux, și folosind un mediu compatibil **sh** pe Windows (Git, WSL, Bash).