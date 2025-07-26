<p align="center">
<img src="https://rares-cosma.github.io/logo.png" alt="Logo" width="200" height="200">
</p>

<h1 align="center">DOCUMENTAȚIE</h1>
<h2 align="center">ROScript</h2>

## ❓ Descrierea problemei

Ca iubitor de informatică, am descoperit două probleme în predarea informaticii în licee, parcurgând primul an de liceu. Din discuțiile cu profesori, olimpici și elevi cu diferite pasiuni, am observat că cele 2 probleme fundamentale în înțelegerea informaticii sunt **barierele de limbaj** și **rigiditatea pseudocodului**.

Problema principală este că pseudocodul nu poate fi *executat*. Majoritatea elevilor ajung să parcurgă manual diferite bucăți de pseudocod, fără a întelege utilitatea acestuia sau a informaticii. Pseudocodul de altfel poate fi găsit în diferite forme, neavând un standard. Când elevii încep să studieze C++, un limbaj complex, nu au bazele necesare, iar ca urmare nu înțeleg informatica.

## 💾 Descrierea soluției

În urma deficiențelor identificate, am decis să dezvolt ROScript, primul limbaj independent în limba română. Acesta poate executa codul asemănator cu pseudocodul folosind un interpretor, facilitând astfel trecerea mai lină de la teorie la practică în C++.

Acesta prezintă un interpretor cu arhitectura Tree-Walk, funcționalități moderne și portabilitate pe majoritatea platformelor. 

## 👥 Publicul țintă

ROScript tinde sa fie un limbaj cât mai accesibil pentru oricine, dar a fost conceput cu acest public țintă:
* Elevii de liceu din clasele IX–XII, în special de la profilurile Matematică-Informatică și Științe ale Naturii
* Persoane care nu cunosc limba engleză, dar vor să poată învăța bazele programării în limba română
* Începătorilor în programare, care își doresc o introducere ușor de înțeles în conceptele de bază

## 🧑‍🏫 Propunere pentru introducerea în sistemul de învățământ

Pentru valorificarea potențialului limbajului propun introducerea ROScript în programa oficială de informatică, după cum urmează:

### 1. În ciclul gimnazial

Elevii de gimnaziu ar putea folosi ROScript pentru un modul, în vederea învățării algoritmicii de bază.

### 2. În liceu, în paralel cu pseudocodul clasic

Consider că predarea clasică a pseudocodului are avantajele sale, dar ar putea fi suplimentată de ROScript. Elevii ar beneficia de logica pseudocodului clasic, în timp ce ar avea posibilitatea să execute și să faca debug codului.

### 3. În liceu, ca introducere în Inteligența Artificială

Inteligența artificială este incontestabil un domeniu de viitor, iar ROScript are capacitățile necesare unei introduceri în AI. Listele dinamice conferă o flexibilitate și o ușurință necesară în predarea inteligenței artificiale. Pentru o aplicație reală, se poate găsi un exemplu de regresie liniară în ```examples/linear_regression.ros```

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

3. Do while/until - Repetă cât timp/până când - structuri repetitive

```
repeta {
    // instructiuni
} cat timp (conditie);

repeta {
    // instructiuni
} pana cand (conditie);
```

4. For - Pentru - structură repetitivă

```
pentru (declarare/atribuire; conditie; atribuire) executa {
    // instructiuni
}
```

### Biblioteca Standard - I/O, matematică, liste, tipuri

```
var x = radp(2); // radacina patrata de 2
x = sirc(x); // transformam x in sir de caractere
afiseaza(x, tip(x)); // afisam x si tipul sau
adauga(lista,3);
afiseaza(345);
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

## 🧱 Arhitectura aplicației

### **Module principale**

* **Lexer / Tokenizer** - acest modul citește codul sursă, și îl împarte pe "tokeni" - structuri de date care contin tipul unui cuvânt cheie, și cuvântul în sine
* **Parserul** - preia tokenii și îi ordonează pe **arbori de sintaxă abstractă (AST)**
* **AST-arborii de sintaxă abstractă** - acești arbori rețin în noduri componentele semnificative ale codului, precum expresii, declarații sau apeluri de funcții. Ei nu conțin toate detaliile sintactice (ex: paranteze sau punct și virgulă), ci doar informația semantică esențială.
* **Interpretorul** - aceast modul parcurge recursiv arborii generați de parser, și execută în ordine instrucțiunile
* **Environment (Mediu de execuție)** – gestionează variabilele și scope-urile. Poate fi gândit ca un „context” activ în care se rulează codul.
* **Biblioteci standard** – conțin funcții predefinite utile în dezvoltare, scrise în română, precum ```afiseaza```, ```citeste```, ```lungime```, etc. Acestea pot fi extinse în viitor cu funcționalități pentru fișiere, grafice, rețele sau AI.

### **Tehnologii / paradigme folosite**

ROScript este un limbaj interpretat, adică nu se traduce în cod binar, ci se execută pe loc. Tipul de interpretor folosit este Tree-Walk interpreter, adică un interpretor care parcurge arborii sintactici direct.

Limbajul este scris în C++, folosind doar biblioteca standard STL, iar executorul pentru teste în Python. Acesta se bazează în special pe **programarea orientată pe obiecte**, din nevoia de **modularitate** și **scalabilitate**. Pentru a asigura aceste standarde, am folosit mai multe design pattern-uri:

1) Probabil cel mai evident: **Interpreter**  
Acest pattern se evidențează în evaluarea diferiților arbori sintactici, unde se apelează recursiv pe aceștia, indiferent de tipul node-urilor.
2) Un pattern de ajutor: **Singleton**  
Am folosit clase singleton în situațiile unde aveam nevoie de funcții ajutătoare, dar aveam nevoie de modularitate.
3) Pattern-ul structural: **Composite**  
Definește structura arborilor de sintaxă abstractă. Toate node-urile pleacă de la aceleasi 2 clase, ```ASTNode``` și ```Expr```.

## 🖥️ Instalare

Sunt 2 metode principale pentru instalarea interpretorului:

### **Prin descărcarea unui executabil precompilat**

Se va descărca executabilul aferent fiecărui sistem de operare din folderul ```/interpreter/bin/```.

Windows: ```/interpreter/bin/ros.exe```

Linux: ```/interpreter/bin/ros```

### **Prin compilarea sursei**

Toate fișierele sursă se află în folderul ```/interpreter/src```, cu excepția fișierului pentru automatizarea testelor. Aceste surse pot fi compilate executând fisierul ```compile.sh``` din folderul ```/interpreter/bin```. Acesta poate fi rulat nativ pe Linux, și folosind un mediu compatibil **sh** pe Windows (Git, WSL, Bash).

### **Configurare**

Prin adăugarea flag-ului ```-p``` după comanda ```ros```, se activează profiler-ul, care arată în mod activ timpii de execuție pentru fiecare tip de instrucțiune în parte.  
Pentru syntax highlighting, există o extensie VSCode numită: ROScript.

## ‼️ Analiza competiției

ROScript este primul limbaj nativ în limba română. Alternative precum rugină (proc macro pentru Rust), Hedy sau Citrine există, dar se folosesc de interschimbarea tokenilor din engleză în romană, iar adesea ele nu traduc în totalitate limbajul. rugină este un dialect neoficial al unui limbaj complex, nepotrivit pentru începători. Citrine și Hedy nu au o traducere completă, dar Hedy oferă materiale didactice foarte bune pentru varianta în limba română. 

Punctele forte pe care le are ROScript în comparație cu alternativele existente ar fi:

* 100% din sintaxă în română
* sintaxa ușoară și adecvată pentru trecerea la C++
* erori în română
* este open-source, deci poate fi ușor îmbunătățit
* suport activ

## ❓ De ce am ales aceste tehnologii?

Pentru implementarea ROScript am ales C++ datorită performanței ridicate, a flexibilității oferite de STL și a controlului detaliat asupra memoriei, aspecte esențiale pentru dezvoltarea unui interpretor.

De asemenea, am folosit GCC ca și compilator, deoarece este open-source, portabil și respectă standardele moderne C++17/20.

## 💡 Opinia mea

Consider că la baza ROScript stă o mâhnire de-a mea că am colegi care încă, după un an de informatică, învață cod în C++ pe de rost, iar eu nu am cum să îi ajut. De acolo, în fond, a plecat toată ideea de dezvoltare a limbajului, și a fost motivația să aduc ROScript unde este acum.  

Am fost mereu fascinat de ideea de a crea un limbaj de programare propriu, fiind impresionat de complexitatea tehnică implicată în dezvoltarea unui asemena ecosistem, și, în același timp, de simplitatea și claritatea metodelor de modularizare și scalabilitate, iar acum, având experiența necesară și ipostaza propice de a schimba ceva cu acest limbaj, am început dezvoltarea ROScript.

Cred că poate aduce un ajutor enorm profesorilor care predau unei clase la început de drum, când încearcă să predea logica din spatele calculatoarelor, și a informaticii, fără a mai încurca elevii cu jargon sintactic.

## 📜 Testimoniale ale utilizatorilor

### **Prof. Dorotheea Faur** - profesor de informatică

În contextul educațional actual, predarea bazelor informaticii și în special a pseudocodului poate fi o sarcină destul de dificilă: elevii sunt captivați cu anevoie de execuția standard (pe tablă sau pe caiet) a porțiunilor de cod, iar algoritmii pe care aceștia îi concep în pseudocod nu pot fi verificați decât pe hârtie. Informatica ce părea foarte atractivă din exterior ajunge să pară complicată și greu de stăpânit odată cu începerea studierii ei, fapt care conduce adesea la abandonarea acestui drum perceput ca fiind neplăcut încă de la primii pași.
ROScript vine în întâmpinarea acestor impedimente, oferind o soluție specifică dezvoltării de produse software - rularea pseudocodului și afișarea rezultatelor. Astfel, elevii pot începe să învețe informatica într-un cadru nativ acestei materii.  
Pe lângă faptul că această modalitate de rezolvare a problemelor de informatică pregătește într-un mod natural trecerea la limbajul C++, utilizat cu precădere în cadrul învățământului preuniversitar românesc existent, ROScript propune o abordare de perspectivă a scrierii de cod, prin funcționaltățile pe care le pune la dispoziția utilizatorilor ca punte de legătură spre domeniul inteligenței artificiale. Sumarizând avantajele aduse, suportul furnizat poate deveni un mijloc de consolidare a modalității curente de predare-învățare a informaticii, dar și o fereastră deschisă către ulterioarele posibilități pentru dezvoltare ale acesteia.

### **Vlad Oprea** - elev

Inițial, am decis să folosesc ROScript ca să pot înțelege mai bine logica din spatele programelor de la școala. Și, surprinzător, chiar s-a prins de mine informatica!!
Aveam la început niște dubii în legătură cu sintaxa, dat fiind faptul că este un limbaj în română, dar când am văzut cât de ușor este de folosit și cât de natural este să programezi cu el, mi-a depășit cu mult așteptările!
În opinia mea acesta este un proiect ca la carte, la care se vede că s-a muncit mult. Recomand pentru toți începătorii care vor să intre în lumea informaticii dar și pentru cei care vor să-și îmbunătățească abilitățile de gândire!!

## 🌇 Dezvoltări ulterioare

Deși ROScript are deja toate funcționalitățile de care are nevoie pentru a emula în totalitate pseudocodul, mai sunt câteva modalități pentru a îmbunătăți ecosistemul. Aș dori să mai adaug aceste lucruri în ordinea enunțării lor, în viitor:

* Bootstraping - sistem de biblioteci externe pentru a putea continua dezvoltarea ROScript în ROScript
* Bytecode interpreter - acesta transformă arborii de sintaxă abstractă într-un limbaj de asamblare virual, care se rulează pe o mașină virtuală
* OOP - paradigma clasică necesară pentru dezvoltarea limbajului
* Extinderea bibliotecii standard cu capabilități de AI, grafică, sunet etc.

## 🙏 Mulțumiri speciale

Chiar dacă eu am scris tot codul ROScript, nu am fost singur, ci am fost sprijinit de o întreagă echipă de specialiști. Pe această cale doresc să îmi adresez recunoștința și cele mai calde mulțumiri următoarelor persoane:

**Prof. Dorotheea Faur** - profesorul meu de la clasă, profesorul meu coordonator, și utilizator ROScript  
```
În primul rând, doresc să îi mulțumesc doamnei profesoare Dorotheea Faur, profesorul meu coordonator, pentru tot sprijinul acordat, de la idei și sugestii de dezvoltare, la testarea anumitor funcționalități ale limbajului, și până la sprijinul în încercarea de a promova limbajul în Ministerul Educației și al Cercetării, pentru o implementare a limbajului în viitor.
```
**Dragoș-Andrei Drăghici** - coleg de clasă, prieten apropiat
```
Țin să îi mulțumesc colegului meu, Dragoș, pentru tot sprijinul acordat de-a lungul dezvoltării ROScript. Are toată recunoștiința mea pentru încurajările oferite atunci când am crezut că nu voi termina acest proiect, pentru sugestiile oferite, direct sau indirect, către dezvoltarea limbajului, precum și pentru corectura acestui document și al celorlalte.
```
**Vlad-Andrei Oprea** - coleg de bancă, pasionat de robotică, utilizator ROScript
```
Vreau să îi mulțumesc colegului meu de bancă, Vlad, pentru insight-urile date de-a lungul dezvoltării limbajului, care au fost definitorii pentru stadiul actual al ROScript. Îi mulțumesc de asemenea pentru toate inițiativele prin care m-a ajutat în dezvoltare, respectiv redactarea unor docstringuri și dezvoltarea unui IDE specific ROScript (abandonat acum).
```

Vreau să mai menționez de altfel **prietenii mei** care au fost aproape mereu, cu sugestii, sfaturi și grijă. **(Robert Bogdan, Borza Teodor, Alessia Arcaș, Vatră Rareș)**

Familia a fost un element foarte important când a venit vorba de dezvoltare, deoarece mi-au asigurat condițiile necesare programării ROScript, de la sistem, la timpul liber și neîntrerupt de care am beneficiat.

În final, vreau sa menționez toate persoanele care m-au ascultat ore în sir când vorbeam despre ROScript, chit că poate nu au înteles toate aspectele discutate. 😅

## 📖 Bibliografie

1. [Purposeful Programming Languages: Non-English Syntaxes](https://dev.to/colin-williams-dev/purposeful-programming-languages-non-english-syntaxes-3ib1) - de ajutor în identificarea altor limbaje asemănătoare
2. [Crafting Interpreters - Robert Nystrom](https://craftinginterpreters.com) - teoria și practica dezvoltării unui limbaj de programare, într-o singură carte
3. [Bază de resurse în interpretoare și compilatoare](https://bernsteinbear.com/pl-resources)

În concluzie... *afiseaza("Salut lume")!*

