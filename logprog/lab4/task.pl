nouns(['book', 'pen', 'table']).
verbs(['is']).
colours(['blue', 'black', 'red']).
sizes(['little', 'big']).

listConcatWithGivenDivider([], Divider, L2, [Divider|L2]).
listConcatWithGivenDivider([H1|T1], Divider, L2, [H1|TRes]) :-
	listConcatWithGivenDivider(T1, Divider, L2, TRes).

check(['on', 'the'|T], on(Noun)) :-
    check(T, Noun).
check(['under', 'the'|T], under(Noun)) :-
    check(T, Noun).
check(['the', Colour|T], object(Noun, colour(Colour))) :- 
    colours(ColList),
    member(Colour, ColList),
    check(T, Noun).
check(['the', Size|T], object(Noun, size(Size))) :- 
    sizes(SizeList),
    member(Size, SizeList),
    check(T, Noun).
check([Size], size(Size)) :- sizes(SizesList), member(Size, SizesList).
check([Colour], colour(Colour)) :- colours(ColList), member(Colour, ColList).
check([Noun], Noun) :- nouns(NounsList), member(Noun, NounsList).

analysis(List, s(L,R)):-
   verbs(Verbs),
   member(Verb, Verbs),
   member(Verb, List),
   listConcatWithGivenDivider(Left, Verb, Right, List),
   check(Left, L),
   check(Right, R),!.
   
