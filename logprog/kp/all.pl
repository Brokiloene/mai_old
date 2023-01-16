:- ['romanovs.pl'].
sibling(A, B) :- child(A, X), child(B, X), A \= B, male(X).

sndCousin(A, B) :- child(A, Parent), child(Parent, Grand), sibling(Grand, Grand2),
    child(Parent2 ,Grand2), child(B, Parent2).

commonChild(Child, Husb, Wife) :- child(Child, Husb), child(Child, Wife), male(Husb), female(Wife).
spouse(Husb, Wife) :- bagof(Child, commonChild(Child, Husb, Wife), _).

relation("son", Son, Parent) :- child(Son, Parent), male(Son).
relation("daughter", Daug, Parent) :- child(Daug, Parent), female(Daug).
relation("brother", Brother, Person) :- child(Brother, Fath), male(Brother), male(Fath), child(Person, Fath), Brother \= Person.
relation("sister", Sister, Person) :- child(Sister, Moth), Sister \= Person, female(Sister), female(Moth), child(Person, Moth).
relation("second cousin", SndCous, Person) :- sndCousin(Person, SndCous).
relation("father", Fath, Person) :- child(Person, Fath), male(Fath).
relation("mother", Moth, Person) :- child(Person, Moth), female(Moth).
relation("husband", Husb, Person) :- spouse(Husb, Person).
relation("wife", Wife, Person) :- spouse(Person, Wife).

mv(A, B) :-
    child(A, B);
    child(B, A);
    sibling(A, B);
    sndCousin(A, B);
    spouse(A, B).

prolong([Fst|T], [New, Fst|T]) :- mv(Fst, New), not(member(New, [Fst|T])).

breadth(Target, [[Target|T]|_], [Target|T]).
breadth(Target, [Fst|SparePaths], Path) :- 
    findall(New, prolong(Fst, New), NewPath),
    append(SparePaths, NewPath, CurPaths), !,
    breadth(Target, CurPaths, Path).

print_rel([]). 
print_rel([_]).
print_rel([A, B|T]) :- relation(Text, A, B), write(Text), write(" - "), print_rel([B|T]).

relationChain(X, Y):-
    breadth(Y, [[X]], P1),
    reverse(P1, P),
    write(X),
    write(" - "),
    print_rel(P), !,
    write(Y),
    write("\n").

print_list([]).
print_list([H]) :- write(H), write(".\n").
print_list([H|T]) :- write(H), write(", "), print_list(T).

deleteAll(_, [], []).
deleteAll(X, [X|T], T2) :- deleteAll(X, T, T2).
deleteAll(X, [Y|T], [Y|T2]) :- Y \= X, deleteAll(X, T, T2).

set([], []).
set([H|T], [H|T2]) :- deleteAll(H, T, T1), set(T1, T2), !.

rel_many(["brothers", "sisters", "second cousins", "sons", "daughters"]).

manyToOne(Rel, Res) :- 
    Rel = "brothers" -> Res = "brother";
    Rel = "sisters" -> Res = "sister";
    Rel = "second cousins" -> Res = "second cousin";
    Rel = "sons" -> Res = "son";
    Rel = "daughters" -> Res = "daughter".

check(["How", "many", Rel, "does", Person, "have", "?"]) :- 
    rel_many(RelList), member(Rel, RelList),
    manyToOne(Rel, Rel2), bagof(Relative, relation(Rel2, Relative, Person), Ans), set(Ans, List),
    length(List, Len), write(Len), write(", including "), print_list(List), !.

check(["Relations", "between", Pers1, "and", Pers2]) :- 
    write(Pers1), relationChain(Pers1, Pers2), 
    write(Pers2), write("\n"), !.

check(["Whose", Relation, "is", Person, "?"]) :- 
    write(Person), write(" is "), relation(Relation, Person, Relative), 
    write(Relation), write(" of "), write(Relative), write("\n").

check(["Who", "is", Relation, "of", Person, "?"]) :- 
    relation(Relation, Relative, Person), write(Relative), write(" is "), 
    write(Relation), write(" of "), write(Person), write("\n").
    
