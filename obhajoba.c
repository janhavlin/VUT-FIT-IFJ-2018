=======================OBHAJOBA PROJEKTU IFJ - 7.12.2018=======================

•představení + zadání
/**
 * Dobrý den, vážená komise, já jsem ... a toto jsou mí kolegové ..., ..., ... 
 * Jsme tým číslo 079 a vybrali jsme si variantu zadání č. 1 - implementaci
 * tabulky symbolů jako Binární Vyhledávací strom.    
 */
•struktura projektu
/**
 * Naší implementaci lze rozdělit na 4 samostatné moduly: scanner, PSA, parser 
 * a generátor a pomocné datové struktury. Analýzu provádíme v jediném průchodu
 * vstupním souborem.
 */
•scanner
    •getToken() - KA
    •returnToken() - buffer
/**
 * Scanner slouží pro lexikální analýzu a v jeho rozhraní jsou funkce getToken()
 * a returnToken(). První ze zmínněných funkcí ze vstupu izoluje jeden lexém
 * pomocí KONEČNÉHO AUTOMATU, určí jeho typ a vrátí jej jako strukturu.
 * Funkce returnToken() slouží pro navrácení tokenu do zásobníku scanneru např.
 * v případě, kdy parser zjistí, že začal číst výraz a musí předat řízení PSA.
 */
•psa 
    •zdola nahoru, výrazy
    •processExpression()
    •precedenční tabulka
    •nedefinované proměnné
/**
 * PSA je modul reprezentující Precedenční Syntaktickou analýzu a zajišťuje 
 * zpracovávání výrazů metodou zdola nahoru. V rozhraní nabízí funkci 
 * processExpression(), kterou volá parser, když chce PSA předat řízení 
 * ve chvíli, kdy očekává výraz. PSA postupně zpracovává tokeny, o které si říká
 * scanneru funkcí getToken() a pomocí PRECEDENČNÍ TABULKY SYMBOLŮ určuje, které
 * pravidlo má v danou chvíli aplikovat. PSA také umí odhalit nedefinovanou 
 * proměnnou ve výrazu a nahlásit sémantickou chybu.
 */ 
•parser
    •komunikace se všemi
    •shora dolů, rekurzivní sestupu
    •syntaktika i sémantika
    •chyba = konec
/**
 * Parser je srdcem programu, protože komunikuje se všemi zbylými moduly
 * a jeho metoda parse() je spouštěna z mainu. 
 * Provádí syntaktickou analýzu shora dolů metodou rekurzivního sestupu. 
 * Pro každý neterminál má jednu funkci, ve kterých rekurzívně provádí 
 * syntaktické kontroly podle jednotlivých gramatických pravidel.
 * Parser současně provádí sémantické kontroly s pomocí tabulky symbolů,
 * ve které jsou uchována klíčová slova, proměnné a funkce, takže odhalí 
 * nedefinované proměnné a funkce, redefinice, špatný počet parametrů, apod.
 * Parser i PSA říkají generátoru jaké funkce má vygenerovat.
 */ 
•generátor
    •generuje 3AC
    •spoj seznam
    •typy operátorů
    •tisk na konci
/**
 * Na popud parseru a PSA generuje instrukce 3-adresného kódu v podobě 
 * obousměrného spojového seznamu, aby mohl vkládat instrukce i zpětně
 * do prostřed. Stará se o veškerou typovou kompatibilitu operátorů.
 * Pokud analýza proběhne v pořádku a je zavolána funkce ILPrintAllInst(),
 * vytiskne všechny instrukce na standardní výstup.
 */ 
•prostředí
/**
 * Programovali jsme v prostředí Visual Studio Code a používali jsme ver-
 * zovací systém git spolu s portálem GitHub. 
 */ 
•komunikace
    •msngr, appear
    •schůzky
/**
 * Pro komunikaci jsme používali především messenger a platformu appear.in,
 * která umožňuje videokonference a sdílení obrazovky, čož bylo velmi
 * praktické při vzájemné spolupráci. Snažili jsme se však i o pravidelné
 * osobní setkání.
 */ 
•shrnutí týmové práce
    •měli jsme začít dříve
/**
 * Práci jsme si chtěli rozložit rovnoměrně, ale jak můžete vidět na grafu 
 * z GitHubu, příliš se nám to nepovedlo. Je to pro nás ponaučením pro příště,
 * protože víme, že jsme si mohli ušetřit stres z časové tísně a že by na
 * projektu ještě bylo co zlepšovat.
 */ 
•implementační nedostatky
/**
 * Funkce nevracejí návratovou hodnotu.
 * Výraz na novém řádku je vyhodnocen jako syntaktický error. (chyba gramatiky)
 * Seznam parametrů lze započít čárkou. (chyba gramatiky)
 * Ukončení po prvním nalezeném erroru. (správně se má zotavit a číst do konce)
 */ 