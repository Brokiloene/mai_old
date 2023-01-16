% Task 2: Relational Data

% The line below imports the data
:- ['one.pl'].

% swi linux

remove_all([], _, []).
remove_all([X|T], X, Ans) :- remove_all(T, X, Ans).
remove_all([Y|T], X, [Y|Ans]) :- X \= Y, remove_all(T, X, Ans).

set([], []).
set([H|T], [H|T1]) :- remove_all(T, H, T2), set(T2, T1).

sum([], 0).
sum([H|T], Ans) :- sum(T, Ans2), Ans is Ans2 + H.

len([], 0).
len([_|T], Cnt) :- len(T, Cnt2), Cnt is Cnt2 + 1.

println(S) :- print(S), nl.

% Напечатать средний балл для каждого предмета

subjects_list(Ans) :- findall(X ,grade(_,_,X,_), All_subjects), set(All_subjects, Ans).
avg(Subject, Ans) :- findall(Mark, grade(_, _, Subject, Mark), Mark_list), sum(Mark_list, Sum), len(Mark_list, Len), Ans is Sum / Len. 
print_avg([H|T]) :- avg(H, Avg), println(H), println(Avg), print_avg(T).
task1() :- subjects_list(S), print_avg(S).

% Для каждой группы, найти количество не сдавших студентов

get_group_nums(Ans) :- findall(X, grade(X, _, _, _), List_unp), set(List_unp, Ans).
%get_group_list(Num, Ans) :- findall(X, grade(Num, X, _, _), Ans1), set(Ans1, Ans).
person_not_passed(Num, Ans) :- findall(X, grade(Num, X, _, 2), Ans1), set(Ans1, Ans).
print_each([H|T]) :- person_not_passed(H, Names), println(H), len(Names, Cnt), println(Cnt), print_each(T).
task2() :- get_group_nums(S), print_each(S).

% Найти количество не сдавших студентов для каждого из предметов

%subjects_list(Ans) :- findall(X ,grade(_,_,X,_), All_subjects), set(All_subjects, Ans).
get_stud_list(Sub, Ans) :- findall(X, grade(_, X, Sub, 2), Ans1), set(Ans1, Ans).
print_cnt([H|T]) :- get_stud_list(H, Names), println(H), len(Names, Cnt), println(Cnt), print_cnt(T).
task3() :- subjects_list(S), print_cnt(S). 
