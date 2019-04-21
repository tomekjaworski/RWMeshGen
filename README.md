## RW Mesh Generator

Generator siatek przestrzennych s�u��cych do symulacji rozk�adu
przenikalno�ci elektrycznej
oraz do rekonstrukcji obraz�w p�askich i wolumetrycznych
 w Elektrycznej Tomografii Poejmno�ciowej (ECT).

Wygenerowana siatka z�o�ona jest z czworo�cian�w (simpleks�w), b�d�cych podstaw�
obliczania r�wna� r�niczkowych metod� element�w sko�czonych (ang. *FEM - finit element method*).
Dla obraz�w p�askich s� to tr�jk�ty.

Generator zaimplementowany ok. 2010 na potrzeby �rodowiska Tomokis Studio.
Szczeg�y �rodowiska dost�pne np. [tutaj](docs/Aplikacja_dla_potrzeb_Banasiak_2010.pdf).


#### Ekran g��wny
Pozwala na okre�lenie parametr�w mechanicznych czujnika w postaci poszczeg�lnych wymiar�w
jego przekroju, g�sto�ci element�w oraz parametr�w materia�owych.

Zielona linia s0-s1-s2-s3 okre�la �redni� lokaln� g�sto�� element�w w funkcji
promienia geometrii czujnika.

![main](docs/main.png)

#### Siatka
Wygenerowan� geometri� mo�na podejrze� *on-line*:

![cap](docs/cap.png)

Program posiada r�wnie� mo�liwo�� eksportu siatki do formatu popularnego �rodowiska
wizualizacji danych naukowych - *VTK*.

Koncepcja algorytmu podzia�u przestrzeni wn�trza czujnika: 
*Nowa metoda rekonstrukcji obraz�w dla potrzeb pojemno�ciowej tomografii procesowej* (rozprawa doktorska): Rados�aw Wajman, 2006.
