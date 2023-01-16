% length, member, append, remove, permute, sublist
% swi linux
len([], 0).
len([_|T], N) :-  length(T, N1), N is N1 + 1.

memb([N|_], N) :- !.
memb([_|T], N) :- memb(T, N).

% append an element to the end of the list
appnd([], VAL, [VAL]) :- !.
appnd([X|T], VAL, [X|T2]) :- add(T, VAL, T2).

% delete the first element
remv([VAL|T], VAL, T) :- !.
remv([X|T], VAL, [X|T2]) :- remv(T, VAL, T2).

permute([], []).
permute(N, [X|T]) :- remv(N, X, R), permute(R, T).

sublst([], _).
sublst([H|T], [H|T2]) :- sublst(T, T2), !.
sublst([H|T], [_|T2]) :- sublst([H|T], T2), !.

% Удаление трех последних элементов без стандартных предикатов
del3([_, _, _], []).
del3([X|T], [X|T2]) :- del3(T, T2), !.

% Удаление трех последних элементов со стандартными предикатами
del3_2(_,[],3).
del3_2(X1,X2) :- len(X1,L), del3_2(X1,X2,L), !.
del3_2([X|T1],[X|T2],L) :- L1 is L-1, del3_2(T1,T2,L1).

% Проверка упорядоченности элементов по возрастанию без стандартных предикатов
check_asc([_]) :- !.
check_asc([X, Y|T]) :- X < Y, check_asc([Y|T]), !.

% Проверка упорядоченности элементов по возрастанию со стандартными предикатами
check_asc2(Z) :- len(Z, 1), !.
check_asc2([X,Y|Z]) :- X<Y, check_asc2([Y|Z]).
