#include <iostream>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <string>
#include <iterator>
#include <fstream>
#include <ostream>
#include <vector>
#include <queue>
#include <ctime>
#include <stack>

using namespace std;

int bookid = 0, personid = 0;// , publisherIDD = 0;

class NoFile{};
inline ostream& operator<<(ostream &it, const NoFile *e)
{
	it << "Ne postoji datoeka sa takvim imenom" << endl;
	return it;
}
class BadNumber{};
inline ostream& operator<<(ostream &it, const BadNumber *b)
{
	it << "Uneli ste los broj" << endl;
	return it;
}

class Member
{
	int memID;
public:
	Member(int mem) :memID(mem){}
	void print(ostream &it) const { it << "Member ID:" << memID; }
	const int myMemID()const { return memID; }
};

class Person{
	int personID = ++personid; //licna karta
	string ime, prezime;
	Member *member;
public:
	int myID() { return personID; }
	friend ostream& operator<<(ostream &it, const Person *p)
	{
		p->print(it); //Odgovor na zamerku, ideja: Mozda u buducnosti bude potrebno modeliranje Membera kao potklase klase Person, ostavka mogucnosti za polimorfno ispisivanje;
		return it;
	}
	void print(ostream &it)const { it << ime << " " << prezime << " " << " ID:" << personID; if (member != nullptr) member->print(it); }
	Person(string im, string prez) :ime(im), prezime(prez), member(nullptr){}
	void setMemID(int mem){ member = new Member(mem); }
	const int myMemID()const
	{
		if (member != nullptr) return member->myMemID();
		else return -1;
	}
};

class SetOfPublishers :unordered_map<int, string>
{
	friend class Book;
	int idpub = 0;
public:
	int isthere(string str){
		for (unordered_map<int, string>::iterator it = begin(); it != end(); ++it)
		{
			if (it->second == str) return it->first;
		}
		return 0;
	}
	int addPublisher(string publisher)
	{
		int publisherID = ++idpub;
		unordered_map<int, string>::iterator it = unordered_map<int, string>::find(publisherID);
		if (it != unordered_map<int, string>::end()) return it->first;
		unordered_map<int, string>::emplace(publisherID, publisher);
		return publisherID;
	}
	string find(int id)
	{
		unordered_map<int, string>::iterator it = unordered_map<int, string>::find(id);
		if (it == unordered_map<int, string>::end()) return "";
		return it->second;
	}
	void removePublisher(int id)
	{
		unordered_map<int, string>::erase(id);
	}
};
SetOfPublishers set;

class Date{
	int yy, mm, dd;
public:
	Date(const Date &&d){ yy = d.yy; mm = d.mm; dd = d.dd; };
	Date(int y, int m, int d) :yy(y), mm(m), dd(d){}
	friend ostream & operator<<(ostream &it, const Date *d)
	{
		it << d->dd << "." << d->mm << "." << d->yy;
		return it;
	}
	friend bool operator>(Date &d1, Date &d2)
	{
		return (d1.yy > d2.yy || (d1.yy == d2.yy && d1.mm > d2.mm) || (d1.yy == d2.yy && d1.mm == d2.mm && d1.dd >= d2.dd));
	}
	friend bool operator<(Date &d1, Date &d2)
	{
		return !(d1 == d2 || d1>d2);
	}
	friend bool operator==(Date &d1, Date &d2)
	{
		return (d1.yy == d2.yy && d1.mm == d2.mm && d1.dd == d2.dd);
	}
	friend bool operator<=(Date &d1, Date &d2)
	{
		return(d1 < d2 || d1 == d2);
	}
	friend bool operator>=(Date &d1, Date&d2)
	{
		return (d1>d2 || d1 == d2);
	}
};

class BorrowingPosition{
public:
	friend ostream& operator<<(ostream &it, const BorrowingPosition *p)
	{
		p->pisi(it);
		return it;
	}
	virtual void pisi(ostream &it) const = 0;
	virtual bool isBookInLibrary()const = 0;
	virtual ~BorrowingPosition(){};
};

class Position :public BorrowingPosition{
	int shelfID, cabinetID, roomID;
public:
	virtual bool isBookInLibrary()const override
	{
		return 1;
	}
	Position(int shelf, int cabinet, int room) :shelfID(shelf), cabinetID(cabinet), roomID(room){}

	void pisi(ostream &it)const override{
		it << "Knjiga je u prostoriji:" << roomID << ", U ormaru:" << cabinetID << ", Na polici:" << shelfID << endl;
	}
	~Position(){};
};

class Autori{
	int brAutora;
	string *imena, *prezimena;
public:
	friend ostream& operator<<(ostream &it, const Autori *a)
	{
		for (int i = 0; i < a->brAutora; i++){
			it << a->imena[i] << " " << a->prezimena[i];
			if (a->brAutora - 1 != i) it << ", ";
			else it << ". ";
		}
		return it;
	}
	Autori(int br, string *im,string *pr) :brAutora(br), imena(im),prezimena(pr){}
	friend bool operator==(const Autori &a1, const Autori &a2)
	{
		bool flag = false;
		if (a1.brAutora == a2.brAutora)
		{
			flag = true;
			for (int i = 0; i < a1.brAutora; i++)
			{
				if (a1.imena[i] != a2.imena[i] || a1.prezimena[i]!= a2.prezimena[i]) { flag = false; break; }
			}
		}
		return flag;
	}
};

class ISBN
{
	string isbn10;
	string isbn13;
	bool tip13;
public:
	bool isbn_13()const { return tip13; }
	ISBN(bool t, string isbn) :tip13(t){
		if (tip13)
			isbn13 = isbn;
		else
			isbn10 = isbn;
	}
	string getISBN()
	{
		if (tip13)
			return isbn13;
		else 
			return isbn10;
	}
	friend ostream& operator<<(ostream &it, const ISBN &isbn)
	{
		if (isbn.tip13)
			it << isbn.isbn13;
		else
			it << isbn.isbn10;
	}
};

enum BookCondition{ NOVA, MALO_OSTECENA, VEOMA_OSTECENA, POTREBNO_ZAMENITI, UKLONJENA, NIJEBILA };
static string niz[6] = { "NOVA", "MALO OSTECENA", "VEOMA OSTECENA", "POTREBNO ZAMENITI", "UKLONJENA", "VAN BIBLIOTEKE" };
static BookCondition nizz[6] = { NOVA, MALO_OSTECENA, VEOMA_OSTECENA, POTREBNO_ZAMENITI, UKLONJENA, NIJEBILA };
class Book{
	int brojIzdanja;
	BookCondition condition;
	BorrowingPosition *p;
	string naziv;
	int nazivIzdavaca;
	int bookID;
	Autori *autor;
	int wrapID = -1;
	int godizd;
	ISBN *isbn; //dodaj u konstruktor;
	string zanr; // dodaj
	string jezik; //dodaj
public:
	string getZanr()const { return zanr; }
	string getLang()const { return jezik; }

	Autori* getAutor()const { return autor; }
	void addWrapper(int id){ wrapID = id; }
	int readWrapper()const { return wrapID; }
	~Book()
	{
		p = nullptr;
		autor = nullptr;
	}
	Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, Autori *aut, string nazivIzd, int podatakOIZD,string isbnn,bool tip, string zanrr,string jezikk) :
		p(pp), naziv(naz), godizd(godinaizd), condition(bc), autor(aut), brojIzdanja(podatakOIZD),jezik(jezikk),zanr(zanrr){
		nazivIzdavaca = set.addPublisher(nazivIzd);
		isbn = new ISBN(tip, isbnn);
	}
	void addBookID(int bkid){
		bookID = bkid;
	}
	void easyPrint(){
		cout << naziv << "," << set.find(nazivIzdavaca) << "-" << autor << "Godina Izdanja " << godizd << endl;
	}
	void pisi(ostream &it)const{
		it << naziv << "-" << autor << " Izdao: " << set.find(nazivIzdavaca) << ". " << endl;
		it << "Stanje: " << niz[condition] << "(" << godizd << ") " << "broj Izdanja: " << brojIzdanja << " ID: " << bookID << endl;
		it << p << endl;
	}
	friend ostream& operator<<(ostream &it, const Book *knjiga){
		knjiga->pisi(it);
		return it;
	}
	BorrowingPosition *location(){
		return p;
	}
	void position()
	{
		if (p != nullptr) cout << p;
	}
	void taken_position(BorrowingPosition *bp)	{ p = bp; }
	int getYear()const{ return godizd; }
	//const int getPublishersID() const {return SetOfPublishers::find(nazivIzdavaca);}
	const string getBooksName()const{ return naziv; }
	const string getPublishersName() const{ return set.find(nazivIzdavaca); }
	const int getBookID()const{ return bookID; }
	int getCondition()const
	{
		return condition;
	}
	void setCondition(BookCondition c){ condition = c; }
};

class Borrowing :public BorrowingPosition{
	Book *knjiga;
	Date *begin, *end;
	bool back = 0;
	int memberID;
	BookCondition stanje;
public:
	~Borrowing(){ end = begin = nullptr; knjiga = nullptr; }
	virtual bool isBookInLibrary() const override
	{
		return 0;
	}
	Borrowing(Book *knj, Date *beg, Date *end, int memID) :
		begin(beg), memberID(memID), end(end), back(0), knjiga(knj){
		stanje = (BookCondition)knj->getCondition();
	}
	const void vratio(BookCondition bcd){ if (stanje > bcd) cout << "Lose Ste Vodili Racuna o knjizi!!!" << endl; back = 1; }
	Date *datumVracanja(){
		return end;
	}
	Date * uzeta(){
		return begin;
	}
	bool vracena(){ return back; }
	Book* book(){ return knjiga; } //pok
	int memID()const { return memberID; }
	void print(ostream &it)const{
		if (knjiga)
		{
			it << "Knjiga:" << knjiga;
			//pisi(it);
		}
	}
	void pisi(ostream &it) const override{
		it << "  Od: " << begin << "  Do: " << end; //a->readMember(memberID) << "  Od: " << begin << "  Do: " << end;
		if (!back) it << endl << "  ****NIJE VRACENA****  " << endl;
	}
	friend ostream&operator<<(ostream &it, const Borrowing b)
	{
		b.print(it);
		return it;
	}
};

static const int T = 100;

class Result
{
	Book **knjige;
	int kap, num;
public:
	void more()
	{
		int kapNew = 2 * kap;
		Book **knjigeNew = new Book*[kapNew];
		for (int i = 0; i < num; i++)
		{
			knjigeNew[i] = knjige[i];
			knjige[i] = nullptr;
		}
		delete knjige;
		knjige = knjigeNew;
		kap = kapNew;
	}
	Result() :kap(10), num(0)
	{
		knjige = new Book*[10];
	}
	void addToResult(Book *knj)
	{
		if (num == kap) more();
		knjige[num++] = knj;
	}
	int getNum()const { return num; }
	friend ostream& operator<<(ostream &it, const Result *r)
	{
		for (int i = 0; i < r->getNum(); i++)
		{
			it << r->knjige[i];
		}
		return it;
	}
};

class BookCollection{
	int wrapperID = 0;
	class BookWrapper{
		/*struct cmpclass{
		bool operator()(const Book &levi, const Book &desni){
		return levi.getCondition() < desni.getCondition();
		}*/
		//};
		//priority_queue<Book*,deque<Book*>, cmpclass> sameBooks;
		int myID, num;
		//string name;
		Book *vector[T];
	public:
		bool IDin(int bookID)const {
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getBookID() == bookID) return true;
			}
			return false;
		}
		string wrapperName()const { return vector[0]->getBooksName(); }
		Autori* wrapperAutors() const { return vector[0]->getAutor(); }
		string wrapperPublisher()const { return vector[0]->getPublishersName(); }
		int zamena()
		{
			int t = 0;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getCondition() == POTREBNO_ZAMENITI) t++;
			}
			return t;
		}
		int pozajmljene()
		{
			int t = 0;
			for (int i = 0; i < num; i++)
			{
				if (!vector[i]->location()->isBookInLibrary()) t++; //knjige u vlasnistvu clanova
			}
			return t;
		}
		int numOfBooks(){
			int t = 0;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->location()->isBookInLibrary()) t++; //knjige trenutno u biblioteci(U posedu biblioteke)
			}
			return t;
		}
		int myIDD()const { return myID; } //mmm?
		friend ostream& operator<<(ostream &it, const BookWrapper *bw)
		{
			for (int i = 0; i < bw->num; i++)
			{
				it << bw->vector[i];
			}
			return it;
		}
		Book* getBook()
		{
			if (!num) return nullptr;
			int min = 0;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getCondition() < vector[min]->getCondition() && vector[i]->getCondition() != UKLONJENA || vector[min]->getCondition() == UKLONJENA)
					min = i;
			}
			Book *get = vector[min];
			if (get->getCondition() == UKLONJENA) return nullptr;
			for (int i = min; i < num - 1; i++)
			{
				vector[i] = vector[i + 1];
			}
			return get;

		}
		void addBook(Book *knjiga)
		{
			if (num >= T) return;
			vector[num++] = knjiga;
		}
		//~BookWrapper(){
		//	delete vector;
		//}
		BookWrapper(Book *first) :num(1)
		{
			vector[0] = first;
			myID = first->readWrapper();
		}
		bool check(Book *knjiga)
		{
			return vector[0]->getAutor() == knjiga->getAutor() && vector[0]->getBooksName() == knjiga->getPublishersName() && vector[0]->getPublishersName() == knjiga->getPublishersName() && vector[0]->getYear() == knjiga->getYear();
		}
		void removeBook(int bookID){
			Book* b = nullptr;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getBookID() == bookID) {
					b = vector[i];
					break;
				}
			}
			if (b) b->setCondition(UKLONJENA);
		}
		Book* look(){
			return vector[num - 1];
		}
		void fillResult(Result *r)
		{
			for (int i = 0; i < num; i++)
			{
				r->addToResult(vector[i]);
			}
		}
		void allDate(Date *d, Result *r)
		{
			for (int i = 0; i < num; i++)
			if (*d == *((Borrowing *)vector[i]->location())->datumVracanja()) r->addToResult(vector[i]);
		}

		void allCondition(BookCondition c, Result *r){
			for (int i = 0; i < num; i++)
			if (c == vector[i]->getCondition()) r->addToResult(vector[i]);
		}

	};
	map<int, BookWrapper*> collection;
public:
	~BookCollection(){
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			delete it->second;
	}
	void addBook(Book *knjiga)
	{
		bool bookPlaced = false;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		{
			if (it->second != nullptr && it->second->check(knjiga)) {
				knjiga->addWrapper(it->second->myIDD());
				it->second->addBook(knjiga);
				bookPlaced = true;
				break;
			}
		}
		if (!bookPlaced) {
			knjiga->addWrapper(wrapperID++);
			collection.emplace(knjiga->readWrapper(), new BookWrapper(knjiga));
		}

	}
	int zamena(){
		int t = 0;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			t += it->second->zamena();
		return t;
	}
	int pozajmljene(){
		int t = 0;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			t += it->second->pozajmljene();
		return t;
	}
	int numOfBooks(){
		int t = 0;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			t += it->second->numOfBooks();
		return t;
	}
	void removeBook(int bookID)
	{
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (it->second->IDin(bookID))	{
			it->second->removeBook(bookID);
			break;
		}
	}
	void printAllBooks()
	{
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			cout << it->second;
	}
	Book* lookAtBook(int bookID)
	{
		map<int, BookWrapper*>::iterator it = collection.find(bookID);
		if (it == collection.end()) return nullptr;
		return it->second->look();
	}
	Result* search_Name_Book(string naziv, string nazizd = "", int godina = 0){
		Result *result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (naziv == it->second->look()->getBooksName())
		{
			if (nazizd != "" && godina != 0){
				if (nazizd == it->second->look()->getPublishersName() && godina == it->second->look()->getYear())
					it->second->fillResult(result);
			}
			else it->second->fillResult(result);

		}
		return result;
	}
	Result* search_Date(Date *d){
		Result *result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (!it->second->look()->location()->isBookInLibrary())
			it->second->allDate(d, result);
		return result;
	}
	Result* search_Condition(BookCondition c){
		Result *result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			it->second->allCondition(c, result);
		return result;
	}
};

class Library{
	static int N;
	class FilesInLibrary{
		Library *a;
		multimap<int, Borrowing *> pozajmio;
	public:
		~FilesInLibrary(){
			for (map<int, Borrowing*>::iterator it = pozajmio.begin(); it != pozajmio.end(); ++it)
				delete it->second; a = nullptr;
		}
		FilesInLibrary(Library *aa) :a(aa){}

		void izmedju(Date *begin, Date *end)
		{
			if (pozajmio.empty()) return;
			for (multimap<int, Borrowing*>::iterator it = pozajmio.begin(); it != pozajmio.end(); ++it)
			if (*begin <= *it->second->uzeta() && *end > *it->second->uzeta() || *end >= *it->second->datumVracanja() && *begin <= *it->second->datumVracanja() || *begin >= *it->second->uzeta() && *end <= *it->second->datumVracanja())
				cout << a->readMember(it->second->memID()) << endl;
		}
		void istekaoRok(Date *end)
		{
			bool ima = false;
			if (pozajmio.empty()) return;
			for (multimap<int, Borrowing*>::iterator it = pozajmio.begin(); it != pozajmio.end(); ++it)
			if (*it->second->datumVracanja() < *end && !it->second->vracena())
			{
				ima = true;
				cout << a->readMember(it->second->memID()) << endl;//*it->second; ;
			}
			if (!ima) cout << "Ni jedan clan nije prekoracio rok do danasnjeg dana." << endl;
		}
		void borrowings(int clanID){
			pair<multimap<int, Borrowing*>::iterator, multimap<int, Borrowing *>::iterator> ret;
			ret = pozajmio.equal_range(clanID);
			if (ret.first == pozajmio.end()) {
				cout << "Clan Jos Nije Pozajmio Nista" << endl; return;
			}
			cout << "clan: " << a->readMember(clanID) << "je pozjamio : " << endl;
			multimap<int, Borrowing *>::iterator it;
			for (it = ret.first; it != ret.second; ++it)
			{
				it->second->book()->easyPrint(); cout << it->second << endl;
			}
		}
		void all(){
			std::multimap<int, Borrowing *>::iterator it;
			for (it = pozajmio.begin(); it != pozajmio.end(); ++it)
			{
				cout << a->readMember(it->first) << ", " << it->second->book();
				if (it->second->vracena()) cout << it->second;
				cout << endl;
			}
		}
		void borrow_Book(int memID, Book *knjiga, Date *beg, Date *end){
			Borrowing *b = new Borrowing(knjiga, beg, end, memID);
			pozajmio.emplace(memID, b);
			knjiga->taken_position(b);
		}
		void return_Book(int memID, Book *knjiga, BookCondition bcd){
			std::pair<std::multimap<int, Borrowing*>::iterator, std::multimap<int, Borrowing *>::iterator> ret = pozajmio.equal_range(memID);
			if (ret.first == pozajmio.end()) {
				cout << "Clan Jos Nije Pozajmio nista" << endl; return;
			}
			std::multimap<int, Borrowing *>::iterator it;
			bool bookFound = false;
			Borrowing *bb;
			for (it = ret.first; it != ret.second; ++it)
			{
				Borrowing *b = bb = it->second;
				if (knjiga == b->book())
				if (!b->vracena()){
					b->vratio(bcd); bookFound = true;
					break;
				}
			}
			if (bookFound) {
				cout << "Unesite Poziciju na koju bi vratili knjigu(prvo Prostorija, Ormar pa policu)" << endl;
				int a, b, c; cin >> a; cin >> b; cin >> c; if (a < 0 || b < 0 || c < 0) throw new BadNumber();
				bb->book()->taken_position(new Position(a, b, c));
			}
		}
	};
	class Member{
		bool ismember;
		int num = 0, licna;
		Person *p;
	public:  //za pocetak.
		~Member(){ p = nullptr; }
		Member(Person *pe) :p(pe), ismember(1){ licna = pe->myID(); }
		friend ostream& operator<<(ostream &it, const Member* m){ it << m->p; return it; }
		bool isMember()const{ return ismember; }
		void delMember(){ ismember = 0; }
		void makeMember(){ ismember = 1; }
		bool good(){ return num == 0; }
		void borrow(){ num++; }
		void returrn(){ num--; }
		bool more(){ return num >= N; }
		int possesion()const{ return num; }
		Person* person(){ return p; }
	};
	unordered_map<int, Member*> members;
	BookCollection *books;
	FilesInLibrary *files;
	Member* check_member(int memID)
	{
		unordered_map<int, Member *>::iterator it = members.find(memID);
		if (it == members.end()) return nullptr;
		return it->second;
	}
	Book* check_book(int bookID)
	{
		if (!books) return nullptr;
		return books->lookAtBook(bookID);
	}												//FileStorage *files;
public:
	int zamena(){
		if (books) return books->zamena();
		else return 0;
	}
	int pozajmljene(){
		if (books) return books->pozajmljene();
		else return 0;
	}
	int numOfBooks(){
		if (books) return books->numOfBooks();
		else return 0;
	}
	~Library(){
		delete files; files = nullptr;
		delete books; books = nullptr;
		for (unordered_map<int, Member*>::iterator it = members.begin(); it != members.end(); ++it)
			delete it->second;
	}
	void search_Name_Book(string naziv, string nazizd = "", int godina = 0){
		Result *r = books->search_Name_Book(naziv, nazizd, godina);
		cout << r;
	}
	void search_Date(Date *d){
		Result *r = books->search_Date(d);
		cout << r;
	}
	void search_Condition(BookCondition c){
		Result *r = books->search_Condition(c);
		cout << r;
	}
	static void setN(int n){ N = n; }
	void istekaoRok(Date *datum){
		if (files) files->istekaoRok(datum);
	}
	void izmedju(Date *begin, Date *end)
	{
		if (files) files->izmedju(begin, end);
	}
	Library() :books(nullptr), files(nullptr){
		members.clear();
	}
	int addMember(Person *p)
	{
		int memberID = p->myID();
		unordered_map<int, Member*>::iterator it = members.find(memberID);
		if (it == members.end()){
			members.emplace(memberID, new Member(p));
			cout << "Uspesno Uclanjenje" << endl;
			return memberID;
		}
		if (it != members.end() && !it->second->isMember()){
			it->second->makeMember();
			cout << "Osoba je vec clan biblioteke" << endl;
			return memberID;
		}
		return 0;
	}
	void addBook(Book *knjiga)
	{
		if (!books) books = new BookCollection();//(this);
		{knjiga->setCondition(NOVA); knjiga->addBookID(++bookid); books->addBook(knjiga); }
	}
	void removeMember(int memID)
	{
		unordered_map<int, Member*>::iterator it = members.find(memID);
		if (it == members.end()) { cout << "Clan Nije Ni Uclanjen" << endl; return; }
		if (it != members.end() && it->second->isMember() && it->second->good()){
			it->second->delMember();
			cout << "Uspesno Isclanjenje" << endl;
		}
	}
	void removeBook(int bookID)
	{
		if (books) books->removeBook(bookID);
	}
	Person *readMember(int memID) ///////////////////CH
	{
		unordered_map<int, Member *>::iterator it = members.find(memID);
		if (it == members.end()) return nullptr;
		return it->second->person();
	}
	void member_borrowings(int memID)
	{
		if (files) files->borrowings(memID);
	}
	void allBorrowed()const
	{
		if (files) files->all();
	}
	void allMembers()
	{
		unordered_map<int, Member*>::iterator it = members.begin();
		cout << "ALL MEMBERS:" << endl;
		for (; it != members.end(); ++it)
		{
			cout << it->second; if (!it->second->isMember()) cout << "(Isclanjen)"; cout << endl;
		}
		cout << endl;
	}
	void allBooks()
	{
		if (books) books->printAllBooks();
	}
	//void help_Borrow(int memID, string s, Date *beg, Date *end)
	//{
	//	Book* knjiga=books->askForBook(s);
	//	borrow(memID, knjiga->getBookID(), beg, end);
	//}
	void borrow(int memID, int bookID, Date *beg, Date *end)
	{
		Member *mem = check_member(memID);
		if (!mem || !mem->isMember()) { cout << "Nema Uclanjenog Sa Takvim ID-om" << endl; return; }
		if (mem->more()){ cout << " Clan vec poseduje " << mem->possesion() << " Sto je i maksimum." << endl; return; }
		Book *knjiga = check_book(bookID);
		if (knjiga == nullptr) { cout << "Nepostojeca Knjiga" << endl; return; }
		if (knjiga->getCondition() == UKLONJENA) { cout << " Knjiga je uklonjena iz upotrebe. " << endl; return; }
		if (knjiga->location()->isBookInLibrary())
		{
			if (!files) files = new FilesInLibrary(this);
			files->borrow_Book(memID, knjiga, beg, end);
			mem->borrow();
			cout << "Uspesno Pozajmljena Knjiga" << endl;
		}
		else cout << "Knjiga Je Trenutno Pozajmljena" << endl;
	}
	void returnBook(int memID, int bookID, BookCondition bcd)
	{
		Member *mem = check_member(memID);
		if (!mem->isMember()) { cout << "Nema Uclanjenog Sa Takvim ID-om"; return; }
		Book *knjiga = check_book(bookID);
		if (!knjiga) { cout << "Nepostojeca Knjiga"; return; }
		if (files) {
			files->return_Book(memID, knjiga, bcd);
			mem->returrn();
		}
	}
	//void allPublishers()const{Books::sviIzdavaci();}
};

int Library::N = 10;
//void Borrowing::pisi(ostream &it) const{
//	it << "Knjiga je u posedu clana: " << a->readMember(memberID) << "  Od: " << begin << "  Do: " << end;
//	if (!back) it << "  ****NIJE VRACENA****  " << endl;
//}

class Instruction{

public:
	virtual void undo() = 0;
	virtual ~Instruction(){}
};

class Ubacivanje_Izbacivanje_Knjiga : public Instruction
{
	int bookID;
	bool ubacivanje;
public:
	void undo() override
	{
		while (true);//radi
	}
	Ubacivanje_Izbacivanje_Knjiga(int id,bool opcija):bookID(id),ubacivanje(opcija){}
	~Ubacivanje_Izbacivanje_Knjiga(){}
};

class Ubacivanje_Izbacivanje_Ljudi : public Instruction
{
	int memberID;
	bool ubacivanje;
public:
	void undo() override
	{
		while (true);//radi
	}
	Ubacivanje_Izbacivanje_Ljudi(int id, bool opcija) :memberID(id), ubacivanje(opcija){}
};

class Promena_Stanja : public Instruction
{
	int bookID;
	BookCondition Prethodno, Novo;
public:
	void undo() override
	{
		while (true); //radi
	}
	Promena_Stanja(int id, BookCondition prvo, BookCondition drugo) :bookID(id), Prethodno(prvo), Novo(drugo){}
	~Promena_Stanja(){}
};
class Pozajmljivanje_Knjige : public Instruction
{
	int bookID;
	int memberID;
	Date *from, *to;
public:
	void undo() override
	{
		while (true); //radi
	}
	Pozajmljivanje_Knjige(int menu, int knjiga, Date *d1, Date *d2) :memberID(menu), bookID(knjiga), from(d1), to(d2){}
	~Pozajmljivanje_Knjige(){
		delete from;
		delete to;
	}
};
class Vracanje_Knjige : public Instruction
{
	int bookID;
	int memberID;
	BookCondition stanje;
public:
	Vracanje_Knjige(int m, int k, BookCondition st) :bookID(k), memberID(m), stanje(st){}
	~Vracanje_Knjige(){}
	void undo() override
	{
		while (true);// radi
	}
};

//class Round_Buffer :private vector<Instruction*>
//{
//	int head, capacity;
//public:
//	Round_Buffer(int cap) :head(0), capacity(cap){}
//	void undo(){
//		if (head != 0)
//		{
//
//			(*this)[head]->undo();
//			delete (*this)[head];
//			(*this)[head--] = nullptr;
//		}
//	}
//	void add(Instruction* ins)
//	{
//		(*this)[++head] = ins;
//	}
//	//void empty()
//	//{
//	//	this->empty();// radi li ?
//	//}
//};

class History:private stack<Instruction*>
{
	Instruction* pop(){
		if (stack<Instruction*>::empty()) return nullptr;
		Instruction *cur = stack<Instruction*>::top();
		stack<Instruction*>::pop();
		return cur;
	}
public:
	void undo()
	{
		Instruction *cur = this->pop();
		if (cur != nullptr) cur->undo();
	}
	void push(Instruction *ins)
	{
		stack<Instruction*>::push(ins);
	}
	~History(){
		while (!stack<Instruction*>::empty())
		{
			Instruction * del = this->pop();
			delete del;
		}
	}
};

class Main{
	History *history; //
	bool rezim;
	int num, brKnjiga;
	Library *biblioteka;
	Person **ljudi;
	Book **knjige;
	ifstream *dnevnik;
	ofstream *upis;
	const string currentDateTime() {
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);

		strftime(buf, sizeof(buf), "%Y-%m-%d, %X", &tstruct);

		return buf;
	}
	void ispisLjudi(Person **ljudi, int broj, Library *biblioteka, bool mode)
	{
		if (mode)
		{
			cout << "Unesite Broj ljudi koje bi uclanili sada: "; 
			*upis << " Uclanjivanje Ljudi U Biblioteku" << endl;
		}
		else {
			cout << "Unesite Broj Ljudi Koje Bi Isclanili sada: ";
			*upis << "Isclanjivanje Ljudi Iz Biblioteke" << endl;
		}
		int k;
		cin >> k;
		if (k <= 0) throw new BadNumber();
		for (int i = 0; i < broj; i++)
		{
			cout << "Broj: " << i << " Za: " << ljudi[i] << endl;
		}
		for (int i = 0; i < k; i++)
		{
			int menu; cin >> menu;
			if (i<k - 1) cout << ", ";
			if (menu < 0 || menu >= broj) throw new BadNumber();
			if (mode) ljudi[menu]->setMemID(biblioteka->addMember(ljudi[menu]));
			else biblioteka->removeMember(ljudi[menu]->myMemID());

			history->push(new Ubacivanje_Izbacivanje_Ljudi(menu, mode));
		}
	}
	void ispisKnjiga(Book **knjige, int broj)
	{
		for (int i = 0; i < broj; i++)
		{
			cout << "Broj: " << i << " Za "; knjige[i]->easyPrint();
		}
		cout << "Unesite Broj Koju Knjigu Birate" << endl;
	}
	void ubacivanjeKnjiga(Library *biblioteka, int broj, Book **knjige, bool mode)
	{
		if (mode) {
			cout << "Unesite Broj Knjiga Koje Zelite Da Ubacite "; 
			*upis << " Ubacivanje Knjiga U Biblioteku" << endl;
		}
		else {
			cout << "Unesite Broj Knjiga Koje Zelite Da Izbacite "; 
			*upis << "Izbacivanje Knjiga Iz Biblioteke" << endl;
		}
		int k;
		cin >> k;
		if (k <= 0) throw new BadNumber();
		ispisKnjiga(knjige, broj);
		for (int i = 0; i < k; i++)
		{
			int izbor; cin >> izbor;
			if (izbor < 0 || izbor >= num) throw new BadNumber();
			if (mode) {
				cout << "Unesite Poziciju Na koju hocete da postavite knjigu(Prvo br.Prostorija, Ormara pa police)" << endl;
				int pr, ore, po; cin >> pr; cin >> ore; cin >> po;
				if (pr <= 0 || ore <= 0 || po <= 0)throw new BadNumber();
				Position *pos = new Position(pr, ore, po);
				knjige[izbor]->taken_position(pos);
				knjige[izbor]->setCondition(NOVA); biblioteka->addBook(knjige[izbor]);
			}
			else biblioteka->removeBook(knjige[izbor]->getBookID());

			history->push(new Ubacivanje_Izbacivanje_Knjiga(izbor, mode));
		}
	}
	void pozicija(Book **knjige, int broj)
	{
		ispisKnjiga(knjige, broj);
		int a;
		cin >> a;
		if (a < 0 || a >= broj) throw new BadNumber();
		knjige[a]->position();
	}
	void pretrazivanje_knjiga(Library *biblioteka)
	{
		cout << "Unesite Opciju :" << endl;
		cout << "1.Pretraga prema nazivu knjige" << endl;
		cout << "2.Pretraga prema isteku datuma" << endl;
		cout << "3.Pretraga Prema Stanju knjige" << endl;
		int menu; cin >> menu;
		switch (menu)
		{
		case 1:
		{
				  cout << "Unesite Naziv Knjige" << endl;
				  string st; cin >> st; cout << endl;
				  cout << "1.Pretraga Samo po nazivu" << endl;
				  cout << "2.Po nazivu,izdavacu i godini izdanja " << endl;
				  int op; cin >> op;
				  if (op == 2)
				  {
					  cout << "Uneiste Izdavaca i Godinu Izdanja." << endl;
					  string s; cin >> s; cin >> op;
					  biblioteka->search_Name_Book(st, s, op);
				  }
				  else { biblioteka->search_Name_Book(st); }
				  *upis << " Pretraga Po Nazivu knjige" << endl;
				  break;
		}
		case 2:
		{
				  cout << "Unesite Datum Pretrage, Dan, Mesec pa Godinu." << endl;
				  int dd, mm, yy; cin >> dd; cin >> mm; cin >> yy;
				  biblioteka->search_Date(new Date(yy, mm, dd));
				  *upis << " Pretraga Po Datumu Pozajmice" << endl;
				  break;
		}
		case 3:
		{
				  cout << "Unesite Neko Od Stanja" << endl;
				  for (int i = 0; i < 6; i++)
					  cout << i << "Za " << niz[i];
				  int b; cin >> b;
				  if (b < 0 || b >= 6) throw new BadNumber();
				  biblioteka->search_Condition(nizz[b]);
				  *upis << " Pretraga Po Stanju Knjige" << endl;
				  break;
		}
		}
	}
	void pretrazivanje_clanova(Library *biblioteka)
	{
		cout << "Unesite Opciju:" << endl;
		cout << "1.Zadavanje datuma, provera da li su do tog dana vratili svi koji su trebali" << endl;
		cout << "2.Zadavanje opsega, provera ko je sve tada pozajmljivao koje knjige." << endl;
		int menu; cin >> menu;
		if (menu == 1){
			cout << "Unesite Datum Pretrage, Dan, Mesec pa Godinu." << endl;
			int dd, mm, yy; cin >> dd; cin >> mm; cin >> yy;
			biblioteka->istekaoRok(new Date(yy, mm, dd));
			*upis << " Pretraga Clanova Na Osnovu Isteka Roka Pozajmice" << endl;
		}
		else {
			cout << "Unesite Pocetni Datum Pretrage, Dan, Mesec pa Godinu." << endl;
			int dd, mm, yy; cin >> dd; cin >> mm; cin >> yy;
			cout << "Unesite krajnji Datum Pretrage, Dan, Mesec pa Godinu." << endl;
			int dd2, mm2, yy2; cin >> dd2; cin >> mm2; cin >> yy2;
			biblioteka->izmedju(new Date(yy, mm, dd), new Date(yy2, mm2, dd2));
			*upis << " Pretraga Clanova Na Osnovu Datuma Pozajmljivanja U Opsegu Datuma " << endl;
		}
	}
	void readFromDat()
	{
		ifstream *datKnjige, *datLjudi;
		datKnjige = new ifstream();
		datLjudi = new ifstream();
		cout << "Unesite Ime datoteke iz koje se citaju Knjige" << endl;
		bool check = 1;
		string imeKnjiga = "knjige.txt", imeLjudi = "ljudi.txt";
		while (check)
		{
			try{
				//cin>>imeKnjiga; cin>>imeLjudi;
				datKnjige->open(imeKnjiga);
				datLjudi->open(imeLjudi);
				if (datKnjige == nullptr || datLjudi == nullptr) throw new NoFile();
				check = 0;
			}
			catch (NoFile *nf)
			{
				cout << nf;
			}
		}
		*datLjudi >> num;
		*datKnjige >> brKnjiga;
		if (num <= 0 || brKnjiga <= 0) throw new BadNumber();
		string s1, s2;
		ljudi = new Person*[num];
		for (int i = 0; i < num; i++)
		{
			*datLjudi >> s1; *datLjudi >> s2;
			ljudi[i] = new Person(s1, s2);
		}
		knjige = new Book*[brKnjiga];
		for (int i = 0; i < brKnjiga; i++)
		{
			int brar, god;
			*datKnjige >> s1; //naziv knjige
			*datKnjige >> god; //godina izdanja
			*datKnjige >> s2; //naziv izdavaca 

			*datKnjige >> brar;
			if (brar <= 0) throw new BadNumber();
			string *imena = new string[brar]; //broj autora
			string *prezimena = new string[brar];
			for (int j = 0; j < brar; j++){
				*datKnjige >> imena[j]; //ime autora
				*datKnjige >> prezimena[j];
			}
			int podatak; //podatak o izdanju
			*datKnjige >> podatak;
			string isbn, zanr, jezik;
			bool tip;
			*datKnjige >> tip;
			*datKnjige >> isbn; 
			*datKnjige >> zanr;
			*datKnjige >> jezik;

			//Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, Autori *aut, string nazivIzd, int podatakOIZD, string isbnn, bool tip, string zanrr, string jezikk) :
			knjige[i] = new Book(NIJEBILA, nullptr, s1, god, new Autori(brar, imena,prezimena), s2, podatak,isbn,tip,zanr,jezik);
		}
		datLjudi->close();
		datKnjige->close();
		datLjudi = datKnjige = nullptr;
		*upis << " Citanje Ulaznih Podataka iz Datoteka" << endl;
	}
	void readFromKeyboard()
	{
		cout << "Prvo unesite broj ljudi koje kreirate i sve potrebne podatke za njih.";
		cin >> num;
		if (num <= 0) throw new BadNumber();
		string s1, s2;
		ljudi = new Person*[num];
		for (int i = 0; i < num; i++)
		{
			cout << "Unesite Ime i Prezime Osobe: ";
			cin >> s1; cin >> s2;
			ljudi[i] = new Person(s1, s2);
			cout << endl;
		}
		cout << "Sad unesite broj knjiga koje kreirate i sve potrebne podatke za njih.";
		cin >> brKnjiga;
		if (brKnjiga <= 0) throw new BadNumber();
		knjige = new Book*[brKnjiga];
		for (int i = 0; i < brKnjiga; i++)
		{

			cout << "Unesite sada Naziv knjige, Godinu Izdavanja, Naziv Izdavaca i Broj Autora respektivno" << endl;
			string s1, s2; int brar, god;

			cin >> s1;
			cin >> god;
			cin >> s2;

			cin >> brar;
			if (brar <= 0) throw new BadNumber();
			string *imena = new string[brar]; //broj autora
			string *prezimena = new string[brar];
			cout << "Unesite Ime Pa Prezime Autora" << endl;
			for (int j = 0; j < brar; j++){
				cin >> imena[j]; //ime autora
				cin >> prezimena[j]; //prezime
			}
			cout << "Unesite Podatak O Izdanju(int)" << endl;
			int podatak;
			cin >> podatak;
			string isbn, zanr, jezik;
			bool tip;
			cout << "Unesite tip ISBN-a, 1 za 13-ocifreni, 0 za 10-ocireni" << endl;
			cin >> tip;
			if (tip != 0 && tip != 1) throw new BadNumber();
			cout << "Unsite ISBN" << endl;
			cin >> isbn;
			if (tip && isbn.length() != 13 || !tip && isbn.length() != 10) 	throw new BadNumber();
			cout << "Unesite zanr" << endl;
			cin >> zanr;
			cout << "Unesite jezik" << endl;
			cin >> jezik;

			//Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, Autori *aut, string nazivIzd, int podatakOIZD, string isbnn, bool tip, string zanrr, string jezikk) :
			knjige[i] = new Book(NIJEBILA, nullptr, s1, god, new Autori(brar, imena,prezimena), s2, podatak, isbn, tip, zanr, jezik);
		}
		*upis<< " Citanje Ulaznih Podataka sa Tastature" << endl;
	}
	int selection()
	{

		int menu = 0;
		cout << "Unesite Neku Od Sledecih Opcija:" << endl;
		cout << "1.Uclanjivanje Ljudi U Biblioteku" << endl;//
		cout << "2.Ubacivanje Knjiga U Biblioteku" << endl;//
		cout << "3.Ispis Svih Knjiga u Biblioteci" << endl;
		cout << "4.Ispis Svih Clanova Biblioteke" << endl;
		cout << "5.Pozicija Odredjene Knjige" << endl;
		cout << "6.Stanje Odredjene Knjige" << endl;
		cout << "7.Promena Stanja Odredjene Knjige" << endl;
		cout << "8.Sve Trenutne Pozajmice" << endl;
		cout << "9.Pretrage Knjiga" << endl;
		cout << "10.Pretrage Clanova" << endl;
		cout << "11.Pozajmljivanje Odredjene Knjige" << endl;
		cout << "12.Isclanjivanje Ljudi Iz Biblioteke" << endl;//
		cout << "13.Izbacivanje Knjige Iz biblioteke" << endl;//
		cout << "14.Ispis Svih Pozajmljivanih Knjiga" << endl;
		cout << "15.Ispis svih knjiga koje je pozajmljivao odredjeni clan" << endl;
		cout << "16.Vracanje Knjige" << endl;
		cout << "17.Stanje u biblioteci" << endl;
		cout << "0.Izlaz Iz Programa" << endl;
		cin >> menu; int a;
		if (menu <= 0) throw new BadNumber(); // || menu >= ?;
		if (upis == nullptr || !upis->is_open()) upis->open("Dnevnik_Rada.txt");
		if (rezim) *upis << "::ADMIN::";
		*upis << currentDateTime();
		switch (menu)
		{
		case 1:{ispisLjudi(ljudi, num, biblioteka, 1); break; }
		case 12:{ispisLjudi(ljudi, num, biblioteka, 0); break; }
		case 2:{ubacivanjeKnjiga(biblioteka, brKnjiga, knjige, 1); break; }/////////////
		case 13:{ubacivanjeKnjiga(biblioteka, brKnjiga, knjige, 0); break; }/////////////
		case 3:{biblioteka->allBooks(); *upis<< " Ispis Svih Knjiga U Biblioteci" << endl; break; }
		case 4:{biblioteka->allMembers();*upis<<" Ispis Svih Clanova u Biblioteci" << endl; break; }
		case 14:{biblioteka->allBorrowed(); *upis<<" Ispis Svih Pozajmica" << endl; break; }
		case 5:{pozicija(knjige, brKnjiga);	*upis<< " Ispis Pozicije Knjige" << endl;break; }
		case 6:{ispisKnjiga(knjige, brKnjiga); cin >> a; cout << niz[knjige[a]->getCondition()]; 
			*upis << "Provera Stanja Odredjene Knjige" << endl; break; 
		}
		case 7:
		{
				  ispisKnjiga(knjige, brKnjiga);
				  cin >> a; cout << "Unesite: " << endl;
				  for (int i = 0; i < 6; i++)
					  cout << i << "Za " << niz[i];
				  int novo; cin >> novo;
				  if (novo < 0 || novo >= 6) throw new BadNumber();
				  BookCondition staro = (BookCondition)knjige[a]->getCondition();
				  knjige[a]->setCondition(nizz[novo]);
				  *upis << "Promena Stanja OdredjeneKnjige" << endl;
				  history->push(new Promena_Stanja(a,(BookCondition)novo,staro));
				  break;
		}
		case 8:{biblioteka->allBorrowed(); break; *upis << " Ispis Svih Pozajmica" << endl; }
		case 9: {pretrazivanje_knjiga(biblioteka); break; }
		case 10:{pretrazivanje_clanova(biblioteka); break; }
		case 11:
		{
				   cout << "Izaberite Osobu Koja Pozajmljuje" << endl;
				   for (int i = 0; i < num; i++)
				   {
					   cout << "Broj: " << i << "Za: " << ljudi[i] << endl;
				   }
				   cin >> menu; //Osoba
				   if (menu < 0 || menu >= num) throw new BadNumber();;
				   cout << "Izaberite sada prema IDu knjige odredjenu knjigu za pozajmljivanje" << endl;
				   biblioteka->allBooks();
				   int knjiga; cin >> knjiga;
				   //cout << "Unesite Od kad do kad zelite da pozajmite knjigu (Dan, Mesec Godina" << endl;
				   int dd, dd2, mm, mm2, yy, yy2; cin >> dd; cin >> mm; cin >> yy; cin >> dd2; cin >> mm2; cin >> yy2;
				   biblioteka->borrow(ljudi[menu]->myMemID(), knjiga, new Date(yy, mm, dd), new Date(yy2, mm2, dd2));///////////

				   history->push(new Pozajmljivanje_Knjige(menu, knjiga, new Date(yy, mm, dd), new Date(yy2, mm2, dd2)));
				   break;
		}
		case 15:
		{
				   cout << "Izaberite Osobu od interesa" << endl;
				   for (int i = 0; i < num; i++)
				   {
					   cout << "Broj: " << i << "Za: " << ljudi[i] << endl;
				   }
				   cin >> menu;
				   if (menu < 0 || menu >= num)throw new BadNumber();;
				   biblioteka->member_borrowings(ljudi[menu]->myMemID());
				   break;
		}
		case 16:
		{
				   cout << "Izaberite Osobu od interesa" << endl;
				   for (int i = 0; i < num; i++)
				   {
					   cout << "Broj: " << i << "Za: " << ljudi[i] << endl;
				   }
				   cin >> menu;
				   if (menu < 0 || menu >= num) throw new BadNumber();;
				   cout << "Uniesite ID knjige od interesa" << endl;
				   biblioteka->allBooks();
				   int knjiga; cin >> knjiga;
				   cout << "Unesite Trenutno stanje knjige(Izaberite Opciju)" << endl;
				   for (int i = 0; i < 6; i++)
				   {
					   cout << "i " << niz[i] << endl;
				   }
				   int st;
				   cin >> st;
				   BookCondition stanje = (BookCondition)nizz[st];
				   biblioteka->returnBook(ljudi[menu]->myMemID(), knjiga, stanje);///////////////
				   history->push(new Vracanje_Knjige(menu,knjiga,stanje)); //MENU ILI LJUDI[MENU]->MYMEMID() ???
				   break;
		}
		case 17:
		{
				   cout << "Ukupno Knjiga u Vlasnistvu Biblioteke:" << biblioteka->numOfBooks() << endl;
				   cout << "Trenutno Pozajmljene" << biblioteka->pozajmljene() << endl;
				   cout << "Istekao rok";  biblioteka->search_Date(new Date(1, 1, 1)); cout << endl;////random date.
				   cout << "Potrebno Zameniti" << biblioteka->zamena() << endl;
		}
		case 0:
			return 0;
		}
		return 1;
	}
	void del()
	{
		delete biblioteka;

		for (int i = 0; i < num; i++)
			delete ljudi[i];
		delete ljudi;

		for (int i = 0; i < brKnjiga; i++)
			delete knjige[i];
		delete knjige;

		biblioteka = nullptr;
		knjige = nullptr;
		ljudi = nullptr;
		num = brKnjiga = 0;
		if (dnevnik != nullptr) dnevnik->close();
		if (upis != nullptr) upis->close();
		dnevnik = nullptr;
		upis = nullptr;
		delete history;
		history = nullptr;

	}
	void dnevnik_Ispis()
	{
		if (!rezim) return;
		if (dnevnik != nullptr) dnevnik->close();
		dnevnik->open("Dnevnik_Rada.txt");
		if (dnevnik == nullptr) cout << "Nema Dnevnika Rada" << endl;
		string s; *dnevnik >> s;
		while (!dnevnik->eof())
		{
			for (int i = 0; i < 4 && !dnevnik->eof(); i++){
				cout << s << " ";
				*dnevnik >> s;
			}
			cout << endl;
		}
		dnevnik->close();
	}
public:
	Main(int rez) :biblioteka(nullptr),rezim(rez){
		upis = new ofstream();
		dnevnik = new ifstream();
	}
	void start(){
		history = new History();
		cout << "Za kreiranje biblioteke potrebno je uneti podrazumevanu vrednost N" << endl;
		int n; cin >> n;
		if (n <= 0 || n > 100) throw new BadNumber();
		biblioteka = new Library();
		Library::setN(n);
		cout << "Biblioteka je Kreirana." << endl;
		cout << "Za ucitavanje Knjiga i clanova iz datoteke unesite 1\n za rucno unosenje podataka unesite 2.";
		int choise; cin >> choise;
		upis->open("Dnevnik_Rada.txt");
		if (rezim) *upis << "::ADMIN::";
		*upis << currentDateTime();
		if (choise == 1) readFromDat();
		if (choise == 2) readFromKeyboard();
		while(selection());
	}
	~Main(){
		del();
	}
};

int main()
{
	cout << "Unesite 0 za Rad u normalnom rezimu(Nije dozvoljeno ispisivanje dnevnika rada) i 1 za Admin rezim rada " << endl;
	bool rezim;
	cin >> rezim;
	try{
		if (rezim != 0 && rezim != 1) throw new BadNumber();
		Main *m = new Main(rezim);
		m->start();
		delete m;
	}
	catch (BadNumber *b)
	{
		cout << b;
	}
	catch (...)
	{
		cout << "***NEOCEKIVANA GRESKA***" << endl;
	}
	//********************************
	//    //}
	//    
	//    Person *vlada = new Person("Vesic", "Vladimir"), *isidora = new Person("Isidora", "Bojovic"), *relja = new Person("Relja", "Veinovic"), *milos = new Person("Milos", "Delevic"), *vanja = new Person("Vanja", "Lazic");
	//    BookCondition A = BookCondition::NOVA, B = BookCondition::MALO_OSTECENA, C = BookCondition::VEOMA_OSTECENA, D = BookCondition::POTREBNO_ZAMENITI, E = BookCondition::UKLONJENA,F=BookCondition::NIJEBILA;
	//    string *s1 = new string("Ivo Andric"), *s2 = new string("Vasko Popa"), *s3 = new string("Hamlet"), *s4 =new string("Dusko Radovic"), *s5 = new string("Dobrica Cosic"), s[3] = { "Milos Crnjanski", "Arsenije Carnojevic", "Cirilo" };
	//    Position *pos = new Position(1, 2, 3);
	//    string izdavaci[10] = { "Izdavac0", "Izdavac1", "Izdavac2", "Izdavac3", "Izdavac4", "Izdavac5", "Izdavac6", "Izdavac7", "Izdavac8", "Izdavac9" };
	//    string imenak[10] = { "Knjiga0", "Knjiga1", "Knjiga2", "Knjiga3", "Knjiga4", "Knjiga5", "Knjiga6", "Knjiga7", "Knjiga8", "Knjiga9" };
	//    Date *datumi[10] = { new Date(2013, 2, 10), new Date(2013, 2, 22), new Date(2013, 3, 10), new Date(2014, 3, 4), new Date(2014, 3, 14), new Date(2014, 5, 4),new Date(2014,6,7),new Date(2015,4,5) };
	//    //Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, string *autori, string nazivIzd, int brautora) :
	//    Book *k = new Book(F, pos, "Ime Knjige", 2013, s1, "Izdavac", 1);
	//    Book *a = new Book(F, new Position(1, 2, 3), imenak[0],2012,s1,izdavaci[0],1);
	//    Book *b = new Book(F, new Position(2, 3, 4), imenak[1], 2012, s2, izdavaci[1], 1), *c = new Book(F, new Position(3, 4, 5), imenak[2], 2013, s3, izdavaci[2], 1);
	//    Book *d = new Book(F, new Position(4, 5, 6), imenak[1], 2010, s4, izdavaci[3], 1), *e = new Book(F, new Position(5, 6, 7), imenak[4], 2013, s5, izdavaci[2], 1);
	//    
	//    //Library biblioteka(3);
	//    vlada->setMemID(biblioteka->addMember(vlada));
	//    isidora->setMemID(biblioteka->addMember(isidora));
	//    relja->setMemID(biblioteka->addMember(relja));
	//    biblioteka->allMembers();
	//    milos->setMemID(biblioteka->addMember(milos));
	//    vanja->setMemID(biblioteka->addMember(vanja));
	//    biblioteka->allMembers();
	//    biblioteka->removeMember(milos->myMemID());
	//    biblioteka->allMembers();
	//    /*a->submitPublisher("Prvi");
	//     b->submitPublisher("Drugi");
	//     c->submitPublisher("Treci");*/
	//    biblioteka->addBook(a);
	//    biblioteka->addBook(b);
	//    biblioteka->addBook(c);
	//    biblioteka->allBooks();
	//    biblioteka->addBook(d);
	//    biblioteka->addBook(e);
	//    biblioteka->allBooks();
	//    biblioteka->removeBook(e->getBookID());
	//    biblioteka->allBooks();
	//    biblioteka->allBorrowed();
	//    cout << endl << endl << endl;
	//    biblioteka->borrow(vlada->myMemID(), a->getBookID(), datumi[0],datumi[1] );
	//    biblioteka->borrow(vlada->myMemID(), b->getBookID(), datumi[1],datumi[2]);
	//    biblioteka->returnBook(vlada->myMemID(), a->getBookID());
	//    biblioteka->borrow(isidora->myMemID(), c->getBookID(), datumi[3],datumi[4]);
	//    biblioteka->borrow(relja->myMemID(), a->getBookID(), datumi[2],datumi[3]);
	//    biblioteka->borrow(isidora->myMemID(), d->getBookID(), datumi[0],datumi[2]);
	//    biblioteka->borrow(relja->myMemID(), d->getBookID(), datumi[1], datumi[4]);
	//    //biblioteka->addBook(e);
	//    biblioteka->returnBook(relja->myMemID(), a->getBookID());
	//    biblioteka->borrow(isidora->myMemID(), a->getBookID(), datumi[0], datumi[2]);
	//    biblioteka->allBooks();
	//    cout << endl;
	//    //biblioteka.member_borrowings(vlada->myMemID());
	//    cout << endl << endl;
	//    biblioteka->allBorrowed();
	//    cout << "Pozajmice Vlade" << endl;
	//    biblioteka->member_borrowings(vlada->myMemID());
	//    //biblioteka->returnBook(isidora->myMemID(), d->getBookID());
	//    biblioteka->istekaoRok(datumi[3]);
	//    biblioteka->izmedju(datumi[0],datumi[2]);
	//    biblioteka->search_1("Knjiga0");
	//    biblioteka->search_1("Knjiga1", "Izdavac1", 2012);
	//    cout << "************" << endl;
	//    biblioteka->search_2(datumi[2]);
	//    biblioteka->search_3(UKLONJENA);
	//******************************
}
