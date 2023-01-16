print_res([]). 
print_res([H|T]):-print_res(T),write(H),nl. 

mv(Old, New) :- append(Cur, [empty, w|T], Old), append(Cur, [w, empty|T], New).
mv(Old, New) :- append(Cur, [b, empty|T], Old), append(Cur, [empty, b|T], New).
mv(Old, New) :- append(Cur, [empty, b, w|T], Old), append(Cur, [w, b, empty|T], New).
mv(Old, New) :- append(Cur, [b, w, empty|T], Old), append(Cur, [empty, w, b|T], New).

depth(Target, [Target|T]) :- print_res([Target|T]).
depth(Target, [H|T]) :- mv(H, Cur), not(member(Cur, T)), depth(Target, [Cur, H|T]).


prolong([Fst|OldPath], New) :- mv(Fst, New), not(member(New, OldPath)).

breadth(Target ,[Path|_]) :- Path = [Target|_], print_res(Path).
breadth(Target, [[Fst|OldPath] |SparePaths]) :- 
    findall([New, Fst|OldPath], prolong([Fst|OldPath], New), NewPath),
    append(NewPath, SparePaths, CurPaths),
    breadth(Target, CurPaths).


iter(St, St, _).
iter(Level, St, End):-
    St < End,
    Cur is St + 1,
    iter(Level, Cur ,End).

lvldepth(Target, [Target|Tail], 0) :- print_res([Target|Tail]).
lvldepth(Target, [CurHead|CurTail], N):- 
    N > 0,
    mv(CurHead, NewState),
    not(member(NewState, [CurHead|CurTail])),
    N1 is N-1,
    lvldepth(Target, [NewState, CurHead|CurTail], N1).

solve(Start, Target) :- 
    write('Depth search result:'), nl,
    get_time(S1),
    depth(Target, [Start]),
    get_time(E1),
    Res1 is E1 - S1,
    write(Res1), nl, nl,
    
    write('Breadth search result:'), nl,
    get_time(S2),
    breadth(Target, [[Start]]),
    get_time(E2),
    Res2 is E2 - S2,
    write(Res2), nl, nl,
    
    write('Iter search result:'), nl,
    get_time(S3),
	iter(Level, 1, 20),
    lvldepth(Target, [Start], Level),
    get_time(E3),
    Res3 is E3 - S3,
    write(Res3).
