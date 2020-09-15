# cpp_hw_13_join_server

join_server
Условие
Разработать асинхронный сервер выполняющий операции пересечения и симметрической разницы над
множествами.
Внутренняя реализация должна предоставить возможность работать с двумя таблицами идентичной и
фиксированной структуры. Одна таблица имеет название A, вторая - B и общую структуру:
{
int id;
std::string name;
}
id является первичным ключом и не может дублироваться в одной таблице.
Для содержимого таблицы A:
id | name
---+----------
0 | lean
1 | sweater
2 | frank
3 | violation
4 | quality
5 | precision
и содержимого таблицы B:
id | name
---+----------
3 | proposal
4 | example
5 | lake
6 | flour
7 | wonder
8 | selection
Необходимо уметь формировать пересечение данных из двух таблиц в виде коллекции:
id | A | B
---+-----------+---------
3 | violation | proposal
4 | quality | example
5 | precision | lake
Необходимо уметь формировать симметрическую разность данных из двух таблиц в виде коллекции:
id | A | B
---+-----------+---------
0 | lean |
1 | sweater |
2 | frank |
6 | | flour
7 | | wonder
8 | | selection
Строки должны следовать в порядке возрастания поля id.
Для пополнения коллекции и выполнения операций над множествами необходимо реализовать следующий
протокол:
INSERT table id name
TRUNCATE table
INTERSECTION
SYMMETRIC_DIFFERENCE
1
