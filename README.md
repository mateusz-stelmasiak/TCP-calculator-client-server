# TCP-calculator-client-server
A basic client-server calculator written in C++. The apps communicate over TCP and implement a protocol assigned by the teacher. Made with [@madragonse](https://github.com/madragonse) as a final project for TS (Technologie Sieciowe).

## Funkcjonalność aplikacji
- nawiązanie połączenia,
- uzgodnienie identyfikatora sesji,
-  wykonywanie operacji matematycznych na dwóch argumentach,
- wykonywanie operacji matematycznych na wielu argumentach:
  - klient przesyła wartości liczbowe oraz operator,
  - serwer zwraca wynik końcowy;
- zakończenie połączenia.
- gdy wartość wyniku wykracza poza zakres zmiennej, powinien zostać zwrócony błąd,
- identyfikator sesji oraz znacznik czasu powinny być przesyłane w każdym komunikacie.

## Opis protokołu
Komunikacja między serwerem a klientem została zrealizowana przy użyciu protokołu tekstowego składającego się z pól o ogólnej strukturze {Klucz: wartość;}. Poniżej przedstawiono budowę pakietu, i wypisano wszystkie wykorzystywane klucze i ich możliwe wartości.

### NAGŁÓWEK
**Operacja: …;**
- dodaj
- odejmij
- mnozenie
- dzielenie
- logarytm
- pierwiastek
- potega
- modulo
  
**Status: …;**
- NULL- dla zapytania wysłanego przez klienta.
- OK- dla odpowiedzi serwera zawierającej poprawny wynik działania.
- {string z opisem błędu}- dla błędu działania/pakietu (wypisane w sekcji obsługiwane błędy).
  
**Indeks: …;**
- <1-100> - stała dla danego połączenia serwer-klient.
- 0 - dla pierwszego pakietu w sesji wysłanego przez klienta.

**ZnacznikCzasu: …;**\
Aktualna dla wysłania pakietu data i godzina. Format: hh:mm:ss/dd.mm.rrrr.

### DANE
**Liczba1: …; Liczba2: …; Liczba3: …; [… ] LiczbaN: …;**\
Argumenty liczbowe potrzebne do obliczeń z zakresu < 0, 2^32)\
W pakiecie zwrotnym od serwera jeśli to pole występuje to jest to Liczba1, która
przechowuje wynik działania.

**Koniec: TAK;**\
Wskazuje na zakończenie ciągu przesyłanych argumentów.

### Obsługiwane błędy
Wysyłane w polu status.
- Błędy pakietu
  - Blad_struktury_paczki
  - Brak_identyfikatora
  - Brak_operacji
  - Brak_statusu
  - Brak_znacznika_czasu
  - Niepoprawny_identyfikator
  - Liczby_poza_zakresem
  - Niepoprawne_argumenty
- Błędy operacji
  - Przepelnienie - dla wyników większych lub równych 2^32
  - Wynik_mniejszy_od_zera
  - Dzielenie_przez_zero
  - Nieznana_operacja
  - Za_duzo_argumentow
  - Za_malo_argumentow
  - Niepoprawny_logarytm
  - Niepoprawny_pierwiastek
  - Przepelnienie_wykladnika

### Przykłady pakietów
[obsługa dodawania]
1. Pakiet do serwera ze zleceniem dodawania (1+1)\
Operacja: dodaj;Status: NULL;Identyfikator: 0;ZnacznikCzasu: 15:00:24/29.10.2019; Liczba1: 1;Liczba2: 1;Koniec: TAK;
2. Pakiet zwrotny od serwera z wynikiem dodawania (2)\
Operacja: dodaj;Status: OK;Identyfikator: 50;ZnacznikCzasu: 15:00:25/29.10.2019;Liczba1: 2;Koniec: TAK;

[obsługa odejmowania]
1. Pakiet do serwera ze zleceniem odejmowania (1-3)\
Operacja: odejmij;Status: NULL;Identyfikator: 50;ZnacznikCzasu: 15:00:27/29.10.2019;Liczba1: 1;Liczba2: 3;Koniec: TAK;
2. Pakiet zwrotny od serwera z błędem odejmowania (Wynik_mniejszy_od_zera)\
Operacja: dodaj;Status:Wynik_mniejszy_od_zera;Identyfikator: 50;ZnacznikCzasu: 15:00:28/29.10.2019;Koniec: TAK;

