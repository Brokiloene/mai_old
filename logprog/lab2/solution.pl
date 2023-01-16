% Place your solution here
del1st(X, [X|Y], Y).
del1st(X, [Y|T], [Y|T1]) :- del1st(X, T, T1).

statement(german,1,[German,Oleg,Dmitry]) :- member(history, Dmitry), not(member(history, German)), not(member(history, Oleg)).
statement(german,2,[German,Oleg,_]) :- member(P1, German), member(P1, Oleg),
	member(P2, German), member(P2, Oleg),
	member(P3, German), member(P3, Oleg), P3\=P2, P2\=P1, P3\=P1.
statement(german,3,[German,Oleg,Dmitry]) :- member(biology, German), member(biology, Oleg),member(biology, Dmitry).
statement(german,4,[German,Oleg,_]) :- member(biology,German), member(chemistry,German), member(biology,Oleg), member(chemistry,Oleg).
statement(german,4,[German,_,Dmitry]) :- member(biology,German), member(chemistry,German), member(biology,Dmitry), member(chemistry,Dmitry).
statement(german,4,[_,Oleg,Dmitry]) :- member(biology,Oleg), member(chemistry,Oleg), member(biology,Dmitry), member(chemistry,Dmitry).

statement(oleg,1,[German,Oleg,Dmitry]) :- member(math, German), member(math, Oleg),member(math, Dmitry).
statement(oleg,2,[German,_,_]) :- member(history, German).
statement(oleg,3,[_,Oleg,Dmitry]) :- member(P1, Dmitry), member(P1, Oleg),
	member(P2, Dmitry), member(P2, Oleg),
	member(P3, Dmitry), not(member(P3, Oleg)).
statement(oleg,4,[German,_,Dmitry]) :- member(chemistry, German),member(chemistry, Dmitry).

statement(dmitry,1,[German,Oleg,Dmitry]) :- member(P, German), member(P, Oleg), member(P, Dmitry), not(statement(german,2,[German,Oleg,_])).
statement(dmitry,2,[German,Oleg,Dmitry]) :- member(math, Dmitry), not(member(math,German)), not(member(math,Oleg)).
statement(dmitry,3,[German,Oleg,Dmitry]) :- Dmitry\=German, German\=Oleg, Dmitry\=Oleg.
statement(dmitry,4,[German,_,Dmitry]) :- not(statement(oleg,4,[German,_,Dmitry])).

check(Pers,List):-
del1st(True1,[1,2,3,4],Rem), del1st(True2,Rem,[Lie1,Lie2]),
statement(Pers,True1,List), statement(Pers,True2,List), not(statement(Pers,Lie1,List)), not(statement(Pers,Lie2,List)).

solve(German,Oleg,Dmitry) :-
del1st(_,[biology,history,chemistry,math],German),
del1st(_,[biology,history,chemistry,math],Oleg),
del1st(_,[biology,history,chemistry,math],Dmitry),
check(german,[German,Oleg,Dmitry]), check(oleg,[German,Oleg,Dmitry]), check(dmitry,[German,Oleg,Dmitry]), !.
