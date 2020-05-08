# OpenGLFramework-3dsLoader

v.1.0.1 2008


![3DS Loader](https://github.com/PlayeRom/OpenGLFramework-3dsLoader/blob/master/imgs/3dsLoader-with-wxWidget.png)

## Wstęp

3DSLoader jest to narzędzie służące do wczytywania plików 3ds, czyli plików stworzonych przy pomocy aplikacji 3ds max (lub innych mogących konwertować do tego formatu). 3DSLoader powstał z myślą o OpenGLFramework (zresztą sam powstał na bazie tego frameworka), głownie do konwersji plików 3ds na format 3DObj.

## Układ współrzędnych

Po uruchomieniu programu 3DSLoader bez problemu powinniśmy zauważyć rysowany układ współrzędnych. Jego środek początkowo znajduje się w centrum ekranu, z którego odchodzą trzy linie. Linia czerwona obrazuje oś X, linia zielona Y a niebieska Z. Domyślnie układ współrzędnych wyświetlany jest w trybie „plus only”, czyli wyświetlane są tylko osie dodatnie. Tryby rysowania osi można zmieniać za pomocą klawisza [A]. Dostępne tryby to już wspomniany „plus only” gdzie wyświetlane są tylko osie dodatnie. Kolejnym trybem jest „all” gdzie rysowane są osie dodatnie i ujemne, oraz tryb „off” gdzie rysowanie układu współrzędnych jest wyłączone.

## Tryby pracy: „view” i „object move”

3DSLoader zawiera jakby dwa tryby pracy a raczej możliwość przesuwania obiektu. O aktualnie ustawionym trybie pracy informuje nas napis na samej górze.

Tryb „view” jest to tzw. tryb widoku gdzie sterowanie strzałkami oznacza sterowanie układem współrzędnym wraz z obiektem i jest to ryb domyślny – ustawiony wraz z uruchomieniem programu. 

Drugi tryb „object move”, czyli przesuwanie obiektem, został stworzony w celu umożliwienia przesuwania obiektu względem jego układu współrzędnych. Czyli w trybie „object move” układ współrzędny nie ulega przesunięciu.

Przełączać się pomiędzy trybami „view” a „object move” możemy za pomocą klawisza F3. Więcej o danych trybach pracy dowiemy się z kolejnych rozdziałów.

## Sterowanie układem współrzędnych

Sterowanie układem współrzędnych polega na obracaniu i przesuwaniu obiektu wraz z jego układem współrzędnych, czyli obracamy i przesuwany niejako „całym światem” lub inaczej mówiąc poruszamy kamerą. Sterowanie układem współrzędnych jest ustawione jako domyślne i jest to tzw. tryb widoku („view”). 
Sterowanie układem współrzędnych zawsze umożliwia nam myszka, niezależnie od tryby pracy „view” czy „object move”. Sterowanie klawiszami jest już uzależnione od trybu pracy. Tak, więc za pomocą myszy możemy obracać wczytanym obiektem co umożliwia nam obejrzenie obiektu z każdej strony. Możemy także przesuwać obiekt w dowolną stronę. Aby obracać obiektem należy poruszać myszą z przytrzymanym lewy przyciskiem. Aby przemieszczać obiekt należy poruszać myszą z przytrzymanym prawym klawiszem. Natomiast za pomocą rolki możemy oddalać i przybliżać obiekt.

Sterować układem współrzędnych możemy także za pomocą klawiatury, ale tylko w trybie widoku i bez możliwości przesuwania. Mianowicie za pomocą strzałek możemy obracać obiektem, za pomocą klawiszy PAGE DOWN i PAGE UP możemy przybliżać i oddalać obiekt.
Zawsze możemy zresetować kąty obrotu i przesunięcie za pomocą klawisza [R].

## Tryb „object move” - przemieszczanie obiektu względem układu współrzędnych

Jak już było wspomniane, tryb „object move” powstał w celu przesuwania obiektu względem układu współrzędnych. Daje nam to możliwość wczytywania przyszłych plików 3DObj z obiektami ustawionymi w dowolnym punkcie układu. Aby upewnić się czy rzeczywiście program pracuje w trybie przesuwania obiektu, na samej górze powinien widnieć napis „[F3] Mode: object move”, jeżeli tak nie jest to należy wcisnąć klawisz [F3]. Po przejściu w tryb „object move” zauważymy także, że pod napisem „[F3] Mode: object move” wyświetlą nam się dodatkowe informacje: „Move seed” oraz poniżej „Move: 0.0, 0.0, 0.0”. Pierwsza informacja („Move seed”) podaje nam aktualną wartość ziarna przesunięcia. Jest to wartość skoku o jaką przesuwany będzie obiekt względem każdej osi układu współrzędnych. Zmniejszać oraz zwiększać wartość „Move seed” możemy za pomocą klawiszy [,] oraz [.]. Następna informacja poniżej „Move seed”, wyświetla nam aktualne przesunięcie względem osi X, Y oraz Z.

Klawiszologia w trybie „object move”.
Strzałki GÓRA / DÓŁ przesuwają obiekt wzdłuż osi Y.
Strzałki LEWO / PRAWO przesuwają obiekt wzdłuż osi X.
Klawisze PAGE UP / PAGE DOWN przesuwają obiekt wzdłuż osi Z.

Oprócz przesuwania, mamy także możliwość obracania obiektem, względem jego układu współrzędnych. Obracać jednak możemy obiekt tylko o 90 stopni oraz obracanie to nie jest uzależnione od trybu pracy i funkcjonuje w obydwu z nich. Tak, więc:
Klawisz [X] – obracanie obiektem o 90 stopni po osi X.
Klawisz [Y] – obracanie obiektem o 90 stopni po osi Y.
Klawisz [Z] – obracanie obiektem o 90 stopni po osi Z.

Mamy także dogodną możliwość wycentrowania obiektu względem układu współrzędnych. Możemy to uczynić za pomocą klawisza [C] dzięki czemu środek obiektu pokryje się z punktem 0, 0, 0 układu współrzędnych. Klawisz [C] działa niezależnie od trybu pracy.

## Pełna klawiszologia

| Klawisz | Tryb pracy „view”   | Tryb pracy „object move”         |
|:-------:|:-------------------:|:--------------------------------:|
| Page Up | oddalenie kamery    | przesuwanie obiektu wzdłuż osi Z |
|Page Down| przybliżenie kamery | przesuwanie obiektu wzdłuż osi Z |
|UP       | obracanie widokiem  | przesuwanie obiektu wzdłuż osi Y |
|DOWN     | obracanie widokiem  | przesuwanie obiektu wzdłuż osi Y |
|LEFT     | obracanie widokiem  | przesuwanie obiektu wzdłuż osi X |
| RIGHT   | obracanie widokiem  | przesuwanie obiektu wzdłuż osi X |
|Home     | skalowanie – pomniejszenie obiektu                     |
|End      | skalowanie – powiększenie obiektu
| A       | przełączanie trybów wyświetlania osi współrzędnych
| B       | włącza/wyłącza przezroczystość (działa bez mapowania wybojów)
| C       | ustawia obiekt w centrum układu współrzędnych
| E       | włącza/wyłącza mapowanie wybojów (emboss bump mapping)
| F       | włączenie/wyłączenie rysowania tylnych ścianek poligonów
| L       | włącza/wyłącza oświetlenie
| M       | zmiana trybu rysowania poligonów
| N       | pokazuje/ukrywa wektory normalne
| R       | restartuje kamerę
| S       | włącza/wyłącza cieniowanie Gourauda
| T       | włącza/wyłącza teksturowanie (działa bez mapowania wybojów)
| X       | obracanie obiektu wokół osi X
| Y       | obracanie obiektu wokół osi Y
| Z       | obracanie obiektu wokół osi Z

## Komendy

Komendy należy wpisywać w konsoli, którą należy otworzyć klawiszem [~]. Poniższa tabela przedstawia listę komend zawartych w 3DSLoaderze (nie są to wszystkie komendy a tylko te dodatkowe):

| Komenda | Opis |
|:-------:|:-----|
| 3ds files | Pokazuje listę (wraz z indeksami) wszystkich plików z rozszerzeniem 3ds, jakie znajdują się w katalogu 3dsobjects.
| load3dsfile X | Wczytuje obiekt ze wskazanego pliku 3ds. Za X należy podać odpowiedni indeks pliku (odpowiednią cyfrę). Indeks pobierzemy za pomocą poprzedniej komendy (3ds files).
| create 3dobjfile | Konwertuje plik 3ds do formatu 3DObj, który to jest obsługiwany przez OpenGLFramework. Domyślnie wierzchołki zapisywane są w floating-pint. Plik 3DObj zostanie zapisany w katalogu objects oraz będzie miał taką samą nazwę co plik 3ds. Jeżeli w pliku 3ds zawartych będzie kilka obiektów, to wszystkie one zostaną zapisane w osobnych plikach 3DObj a ich nazwy będą indeksowane.
| create 3dobjfile fixed |Konwertuje plik 3ds do formatu 3DObj, z zapisem wierzchołków w fixed-point (np. dla OpenGL Embedded System). Plik 3DObj zostanie zapisany w katalogu objects oraz będzie miał taką samą nazwę co plik 3ds z postfiksem „_fixed”. Jeżeli w pliku 3ds zawartych będzie kilka obiektów, to wszystkie one zostaną zapisane w osobnych plikach 3DObj a ich nazwy będą indeksowane.
| loadtxt X | Wczytuje obiekt wraz z teksturami z danego pliku tekstowego. Za x należy podać odpowiednią nazwę pliku wraz z rozszerzeniem. Przykładowa zawartość pliku txt poniżej tabeli*.
| x = x | Zamiana obsługi myszki dla obrotu obiektu. Poruszając myszką po osi X ekranu (czyli na boki), obiekt będzie obracał się po osi X swojego układu współrzędnych.
| x = y |Zamiana obsługi myszki dla obrotu obiektu. Poruszając myszką po osi X ekranu (czyli na boki), obiekt będzie obracał się po osi Y swojego układu współrzędnych.
| x = z | Zamiana obsługi myszki dla obrotu obiektu. Poruszając myszką po osi X ekranu (czyli na boki), obiekt będzie obracał się po osi Z swojego układu współrzędnych.
| y = x | Zamiana obsługi myszki dla obrotu obiektu. Poruszając myszką po osi Y ekranu (czyli w górę i dół), obiekt będzie obracał się po osi X swojego układu współrzędnych.
| y = y | Zamiana obsługi myszki dla obrotu obiektu. Poruszając myszką po osi Y ekranu (czyli w górę i dół), obiekt będzie obracał się po osi Y swojego układu współrzędnych.
| y = z | Zamiana obsługi myszki dla obrotu obiektu. Poruszając myszką po osi Y ekranu (czyli w górę i dół), obiekt będzie obracał się po osi Z swojego układu współrzędnych.
| show normal | Rysuje/ukrywa wektory normalne obiektu.
| normal+ | Wydłuża dwukrotnie wektory normalne. Jest to tylko efekt wizualny – wektory normalne cały czas pozostają znormalizowane.
| normal- | Skraca dwukrotnie wektory normalne. Jest to tylko efekt wizualny – wektory normalne cały czas pozostają znormalizowane.

### Przykładowa zawartość pliku txt dla komendy wczytania obieku z pliku txt:

```
; komentarz
3ds = 3dsobjects/space_ship.3DS
texture = textures/space_ship.bmp
bump = textures/space_ship_bump.bmp
```

Istotne są słowa kluczowe: 3ds, texture oraz bump. Przy 3ds, koniecznie po znaku równości podajemy nazwę pliku 3ds, których chcemy wczytać. Przy texture, koniecznie po znaku równości podajemy nazwę tekstury bazowej. Przy bump, koniecznie po znaku równości, podajemy nazwę tekstury dla efektu mapowania wybojów. Jeżeli nie mamy lub nie zamierzamy używać jakiejś tektury to po znaku równości wpisujemy znak ?, np. bez użyta tekstury bump:

```
; komentarz
3ds = 3dsobjects/space_ship.3DS
texture = textures/space_ship.bmp
bump = ?
```