/*
Higher beings, this code is for you alone.
*/
#include "render.cpp"
#include "inputium.cpp"

/*
Pieniadze - start 100 + 10/tura

Jednostki specjalne
Farmer - 10g - Buduje na swoim tile farme (Farma = +1/tura)
Motoryzacja - 25g - Rusz się na jednostkę sojuszniczą (nie czołg) aby dać mov + 2, hp + 20, dmg + 5

Jedonski wojskowe
Piechota - 10g 10hp 5dmg 1mov range 0
Czolgi lekkie - 50g 30hp 10dmg 3mov range 0
Czolgi srednie - 100g 50hp 15dmg 2mov range 0
Czolgi ciezkie - 200g 90hp 20dmg 1mov range 0
Artyleria - 50g 5hp 10dmg 1mov range 3 (o ile sie nie rusza)
Anti tank - 75g 5hp 5dmg(*6 na Czolgi) 1mov range 2 (o ile sie nie rusza)
Marines - 25g 10hp 5dmg 2mov range 0 - moga sie poruszac przez rzeke

Rozkazy wojskowe

Paratroopers - 50g, staty piechoty  - Wybierz pole. Przeciwnik dostaje informacje o tym zakupie. W twojej kolejnej turze na tym polu pojawia sie 
jednostka piechoty (jezeli pole jest zajete przez jednostke walczy z ta jednostka az jedna z nich nie zginie)
Naloty - kazdy ma zasieg i obrazenia. wszystkie jednoski w zasiegu dostaja obrazenia na poczatku twojej kolejnej tury
    1x1 - 100dmg 50g
    3x3 - 50dmg 75g
    5x5 - 50dmg 200g

Nacje

Polska - Jednostka wojskowa: Husaria (25g 20hp 3mov 10dmg range 0)
ZSRR - Twoja piechota kosztuje o 2g mniej
Niemcy - Twoje czołgi średnie są o 1 mov szybsze
USA - naloty tańsze o 15g
Francja - wszystkie jednostki mają +1mov wykonując ruch do tyłu
Japonia - sepcalny rozkaz wojskowy: Kamikaze (30g paratropers, znika po 1 turze)
Mali - Po 10 turach dostajesz + 1 golda
Wielka Brytania - Jednostka wojskowa: Czołgi morskie (75g 30hp 10dmg 2mov range 0, mogą przekraczać rzekę)
Jugosławia - Dostajesz możliwość rozkładania min (20g, obok twoich jednostek.) Gdy dowolna jednostka wjedzie na to pole dostaje 80dmg
Wietnam - Jednostka Specjalna: Naturalista 30g (może kłaśc drzewa ukrywające twoje jednostki i dające +5 dmg)
Chiny - Co 15 tur dostajesz +1g za 3 farmy
Włochy - Jednostka wojskowa: Czołg przeciw pancerny (75g 15hp 10dmg *2na czolgi mov 2 range 1)

5g za pkt kontrolny

*/