#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/****************************/
/*							*/
/*	     Projekt c.2		*/
/*	Praca s datovymi typmi	*/
/*	   Patrik Sehnoutek		*/
/*		 ( xsehno01 )	    */
/*     	  6.12.2020			*/
/*							*/
/****************************/ 

#define MAX 1000

typedef struct
{
	char *cellContent;
	int length;
	int cap;
	bool containsDel;
}cell_t;

typedef struct
{
	cell_t *columns;
	int colCount;
}row_t;

typedef struct
{
	row_t *rows;
	int rowCount;	
	int maxCol;
}table_t;

typedef struct
{
	char *delimiters;
	int n;
	int cap;
}delim_t;

typedef struct
{
	int row;
	int col;
}cellPos_t;

typedef struct
{
	char *name;
	char *parameters;
}command_t;

typedef struct
{
	int length;
	command_t *array;
} sequence_t;

typedef struct
{
	cellPos_t *cells;
} selection_t;

/**
 * cellPos struct constructor
 */
void cell_pos_ctor(cellPos_t *cPos)
{
	cPos->row = 0;
	cPos->col = 0;	
}

/**
 * Command struct constructor
 */
void commandCtor(command_t *command)
{
	command->name = NULL;
	command->parameters = NULL;
}

/**
 * Selection struct constructor
 */
void selectionCtor(selection_t *selected)
{
	selected->cells = NULL;
}

/**
 * Set values to selection struct
 */
void selectionSet(selection_t *selected, int row, int col)
{
	if (selected->cells == NULL)
		selected->cells = malloc(sizeof(cellPos_t) * 2);

	selected->cells[0].row  = row;
	selected->cells[0].col = col;
	selected->cells[1].row = row;
	selected->cells[1].col = col;
}

/**
 * Delim struct constructor.
 */
void delimCtor(delim_t *del)
{
	del->delimiters = NULL;
	del->n = 0;
	del->cap = 0;
}

/**
 * Delim struct inicialization.
 */
int delimInit(delim_t *del)
{
	delimCtor(del);
	
	del->delimiters = malloc(sizeof(char) * 2);
	if (del->delimiters == NULL)
		return -1;
    
	del->delimiters[0] = ' ';
	del->n = 1;
	del->cap = 2;

	return 0;
}

/**
 * Delim struct destructor.
 */
void delimDtor(delim_t *del)
{
	free(del->delimiters);
	del->n = 0;
	del->cap = 0;				
}

/**
 * Check for duplicate delimiters.
 */
bool isDuplicate(delim_t *del, char c)
{
	for (int i = 0; i < del->n; i++)
	{
		if (c == del->delimiters[i])
			return true;
	}
	return false;
}

/**
 * Add delimiter to delim struct.
 */
int addDelim(delim_t *del, char c)
{
	del->n++;
	if (del->n >= del->cap)
	{		
		del->cap = (del->cap)*2;
		char *tmp = realloc(del->delimiters, del->cap*sizeof(char));
			
		if (tmp == NULL)
			return -1;
		del->delimiters = tmp;
	}
	del->delimiters[del->n-1] = c;

	return 0;
}

/**
 * Check if delim is correct.
 */
int checkDelim(delim_t *del, char *s1, char *s2)
{
	if ((strcmp(s1, "-d")) != 0)
		return 0;
	
	if (strchr(s2, '\\') != NULL || strchr(s2, '\"'))
	{
		fprintf(stderr, "Invalid delim. Delim contains special characters.\n");
		return -1;
	}

	del->delimiters[0] = s2[0];
	
	int len = strlen(s2);
	for (int i = 1; i < len; i++)
	{
		bool duplicate = isDuplicate(del, s2[i]);

		if (!duplicate)
			if (addDelim(del, s2[i]) == -1) 
			{
				fprintf(stderr, "Realloc error.\n");
				return -1;
			}
	}
	return 0;
}

/**
 * Check if input file exists
 */
int checkInputFile(char *fileName)
{
	FILE *file;

	file = fopen(fileName, "r");
	if (file == NULL)
		return -1;
	if (fclose(file) == EOF)
		return -1;

	return 0;
}

/**
 * Check if the given char is delimiter. 
 * return: true/false
 */
bool isDelim(delim_t *del, char c)
{
	for (int i = 0; i < del->n; i++)
	{
		if (c == del->delimiters[i])
			return true;
	}
	return false;
}

/**
 * Cell constructor.
 */
void cellCtor(cell_t *cell)
{	
	cell->cellContent = NULL;
	cell->length = 0;
	cell->cap = 0;
	cell->containsDel = false;
}

/**
 * Cell destructor.
 */
void cellDtor(cell_t *cell)
{
	free(cell->cellContent);
	cellCtor(cell);
}

/**
 * Realloc memory for a cell.
 */
int cellRealloc(cell_t *cell, int n)
{	
	char *tmp = realloc(cell->cellContent, n * sizeof(char));

	if (tmp == NULL) return -1;
	cell->cellContent = tmp;
	cell->cap = n;

	return 0;
}

/**
 * Set a given string to the cells.
 */
int cellCset(cell_t *cell, char *string, delim_t *del)
{
	if (cell->cellContent != NULL)
		cellDtor(cell);

	if (cellRealloc(cell, 2) == -1) return -1;
	
	int len = strlen(string);	
	for (int i = 0; i < len; i++)
	{
		if (isDelim(del, string[i]))
			cell->containsDel = true;
		if (i == cell->cap-1)
			cellRealloc(cell, cell->cap * 2);
		
		cell->cellContent[i] = string[i];
		cell->length++;
		if(i + 1 == len) cell->cellContent[i+1] = '\0';
	}	
	return 0;
}

/**
 * Table inicialization.
 */
int tableInit(table_t *table)
{
	table->maxCol = -1;
	table->rows = malloc(sizeof(row_t));
	if (table->rows == NULL)
		return -1; 
	else
		table->rowCount = 1;

	table->rows[0].columns = malloc(sizeof(cell_t));
	if (table->rows[0].columns == NULL)
		return -1;
	else
		table->rows[0].colCount = 1;

	cellCtor(&table->rows[0].columns[0]);
	
	return 0;
}

/**
 * Add columns to a row.
 */
int addColumns(table_t *table, int row, int n, int from)
{
	if (from == 0)
		table->rows[row-1].columns = NULL;
	
	if (table->rows[row-1].colCount < n)
	{
		cell_t *cell = realloc(table->rows[row-1].columns, n * sizeof(cell_t));
		if (cell == NULL) return -1;
			
		table->rows[row-1].columns = cell;	
		table->rows[row-1].colCount = n;
	
		// Cell constructor
		for (int i = from; i < n; i++)
			cellCtor(&table->rows[row-1].columns[i]);
	}
	return 0;
}

/**
 * Add an empty row to the table.
 */
int addRow(table_t *table, int row, int n)
{
	if (table->rowCount < row)
	{	
	row_t *r = realloc(table->rows, row * sizeof(row_t));
		if (r == NULL) return -1;
			
		table->rows = r;
		(table->rowCount)++;
		table->rows[row-1].colCount = 0;		
	}		
	if(addColumns(table, row, n, table->rows[row-1].colCount) == -1) return -1;
	return 0;
}

/**
 * Realloc memory for the table.
 */
int tableRealloc(table_t *table, int n, int m)
{
	for (int i = 1; i <= m; i++)
		if(addRow(table, i, n) == -1) return -1;
	return 0;
}

/**
 * Free memory.
 */
int tableFree(table_t *table)
{
	if (table->rows == NULL) return 0;
	
	for (int i = 0; i < table->rowCount; i++)
	{
		for (int j = 0; j < table->rows[i].colCount; j++)
		{
			free(table->rows[i].columns[j].cellContent);
		}
		free(table->rows[i].columns);
	}
	free(table->rows);

	return 0;
}

/**
 * Load table from a file to table_t structure.
 */
int loadTable(table_t *table, char *fileName, delim_t *del)
{
	FILE *file;
	if ((file = fopen(fileName, "r")) == NULL)
	{
		fprintf(stderr, "%s %s\n",  "Cannot open the file: ", fileName);
		return -1;
	}
	int row = 1;
	int column = 1;
	int pos = 0;
	int c = getc(file);
	bool escape = false;
	bool oneTable = false;

	while(c != EOF)
	{
		// Detect special character 
		if (c == '\\')
			escape = true;
		else if (!escape && c == '\"')
			oneTable = !oneTable;
		// Char is delimiter
		else if (isDelim(del, c) && !escape && !oneTable)
		{	
			if (addColumns(table, row, column+1, column)  == -1) return -1;
			column++;
			pos = 0;
		}
		// The end of the row
		else if (c == '\n')
		{
			if(table->maxCol < column) table->maxCol = column;
			pos = 0;
			row++;
			column = 1;
			// Initialize a new row
			if(addRow(table, row, column) == -1) return -1;
			if (escape) escape = !escape;
		}

		// Inserting char
		else
		{
			// Realloc memory if there left no memory to insert char
			if (pos >= table->rows[row-1].columns[column-1].cap-1)
			{
				int new_cap = (table->rows[row-1].columns[column-1].cap) * 2;
				if (new_cap == 0) new_cap = 2;
				
				cellRealloc(&(table->rows[row-1].columns[column-1]), new_cap);
			}
			// Insert char to cell
			table->rows[row-1].columns[column-1].cellContent[pos] = c;
			table->rows[row-1].columns[column-1].length++;
			pos++;

			if (isDelim(del, c) || c == '\"')
				table->rows[row-1].columns[column-1].containsDel = true;
			if (escape) escape = !escape;
		}
		c = getc(file);

		if (c == EOF)
		{	
			free(table->rows[row-1].columns);
			(table->rowCount)--;
		}
	}
	if (fclose(file) == EOF)
		return -1;
	
	if (oneTable)
		return -1;		
	return 0;
}

/**
 * Print changed table to the file.
 */
int printTable(table_t *table, delim_t *del, char *fileName)  
{
	FILE *file;
	if ((file = fopen(fileName, "w")) == NULL)
	{
		fprintf(stderr, "%s %s\n",  "Cannot open the file: ", fileName);
		return -1;
	}
	
	for (int i = 0; i < table->rowCount; i++)
	{
		for (int j = 0; j < table->rows[i].colCount; j++)
		{
			int len = table->rows[i].columns[j].length;
			// print quotation mark before cell, which contains delimiter
			if (table->rows[i].columns[j].containsDel)
				fprintf(file, "%c", '\"');
	
			for (int k = 0; k < len; k++)		
			{
				char c = table->rows[i].columns[j].cellContent[k];
				// Escape special characters
				if (c == '\\' || c == '\"')
					fprintf(file, "%c", '\\'); 
				fprintf(file, "%c", c);
			}
			// print quotation mark after cell, which contains delimiter
			if (table->rows[i].columns[j].containsDel)
					fprintf(file, "%c", '\"');

			if (j != table->rows[i].colCount-1)
				fprintf(file, "%c", del->delimiters[0]);
		}
		fprintf(file, "%s", "\n");
	}
	
	if (fclose(file) == EOF)
		return -1;
	return 0;
}

/**
 * Sequence constructor.
 */
void sequenceCtor(sequence_t *cmds)
{
	cmds->array = NULL;
	cmds->length = 0;
}

/**
 * Realloc memory for a sequence of commands.
 */
int sequenceRealloc(sequence_t *cmds, int n)
{
	command_t *tmp;
	if (cmds->array == NULL)
		tmp = malloc(sizeof(command_t));
	else
		tmp = realloc(cmds->array, n * sizeof(command_t));
	
	if (tmp == NULL) return -1;
	cmds->array = tmp;

	for (int i = cmds->length; i < n; i++)
		commandCtor(&cmds->array[i]);
	cmds->length = n;

	return 0;
}

/**
 * Free memory.
 */
int sequenceFree(sequence_t *cmds)
{
	for (int i = 0; i < cmds->length; i++)
	{	
		if (cmds->array[i].name != NULL)
			free(cmds->array[i].name);
		if (cmds->array[i].parameters != NULL)
			free(cmds->array[i].parameters);
	}free(cmds->array);

	return 0;
}

/**
 * Realloc memory for a command.
 */
int commandRealloc(command_t *cmd, int choice, int n)
{
	char *tmp;
	if (choice == 0)
	{
		if (cmd->name == NULL)	
			tmp = malloc(n * sizeof(char));
		else
			tmp = realloc(cmd->name, n * sizeof(char));

		if (tmp == NULL) return -1;
		cmd->name = tmp;
	}
	else if(choice == 1)
	{
		if (cmd->parameters == NULL)	
			tmp = malloc(n * sizeof(char));
		else
			tmp = realloc(cmd->parameters, n * sizeof(char));

		if (tmp == NULL) return -1;
		cmd->parameters = tmp;
	}
	
	return 0;
}

/**
 * Change the given string according to the rules.
 */
int editString(char *string)
{
	char tmp[MAX+1];
	memset(tmp, '\0', MAX+1);
	bool escape = false;
	bool oneTable = false;
	int pos = 0;

	int len = strlen(string);
	for (int i = 0; i < len; i++)
	{
		if(string[i] == '\\')
			escape = true;
		else if (!escape && string[i] == '\"')
			oneTable = !oneTable; 
		else
		{
			tmp[pos] = string[i];
			escape = false;
			pos++;
		}			
	}	
	memset(string, '\0', MAX+1);
	strcpy(string, tmp);

	if (oneTable) return -1;
	return 0;
}

/**
 * Convert string containing commands to the sequence of commands.
 */
int convertCommands(char *argv, sequence_t *commands)
{
	sequenceCtor(commands);
	if (sequenceRealloc(commands, 1) == -1) return -1;
	
	int pos = 0;
	int characterCount = 0;
	int commandsCount = 1;	
	char tmp[MAX+1];	
	memset(tmp, '\0', MAX+1);

	int len = strlen(argv);
	for (int i = 0; i <= len; i++)
	{
		if (argv[i] == ' ' && tmp[0] != '[')
		{
			commandRealloc(&commands->array[commandsCount-1], 0, pos+1);
			strcpy(commands->array[commandsCount-1].name, tmp);
			memset(tmp, '\0', MAX+1);
			pos = 0;
		}
		else if(argv[i] == ';'|| i == len)
		{
			if (commands->array[commandsCount-1].name == NULL)
			{
				commandRealloc(&commands->array[commandsCount-1], 0, 10);
				if (tmp[0] == '[')
					strcpy(commands->array[commandsCount-1].name, "selection");
				else
					strcpy(commands->array[commandsCount-1].name, "editing");
			}	
			commandRealloc(&commands->array[commandsCount-1], 1, pos+1);

			// change string according to the rules
			if (editString(tmp) == -1) return -1;
			strcpy(commands->array[commandsCount-1].parameters, tmp);
			memset(tmp, '\0', MAX+1);
			pos = 0;
			
			if (i != len)
			{
				commandsCount++;
				sequenceRealloc(commands, commandsCount);
			}
		}
		else
		{
			tmp[pos] = argv[i];
			pos++;
		}		
		characterCount++;
		 
		if (characterCount == MAX)
		{
			fprintf(stderr, "Exceeded characters limit in a command/-s. Character limit is %d.\n", MAX);
			return -1;
		}
		if (commandsCount > 1000)
		{
			fprintf(stderr, "Exceeded number of commands. Limit of commands is %d.\n", MAX);
			return -1;
		}
	}	
	return 0;
}

/**
 * Find the given string in the selected cells. If string is found, change
 * the selection.
 */
void findStr(table_t *table, selection_t *selected, command_t *cmd)
{
	char tmp[MAX+1];
	memset(tmp, '\0', MAX+1);
	strncpy(tmp, cmd->parameters+6, strlen(cmd->parameters)-7);			

	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)	
		{
			if (table->rows[i-1].columns[j-1].cellContent != NULL)
				if (strstr(table->rows[i-1].columns[j-1].cellContent, tmp))
				{
					selectionSet(selected, i, j);
					return;
				}	
		}
	}
}

/**
 * Copy positions of cells.
 */
void changeCells(selection_t *dst, selection_t *from)
{
	if (from->cells[0].row != 0)
	{
		dst->cells[0].row = from->cells[0].row;
		dst->cells[0].col = from->cells[0].col;
		dst->cells[1].row = from->cells[1].row;
		dst->cells[1].col = from->cells[1].col;
	}
}

/**
 * Find the biggest number in the selected cells. If number is found, change
 * the selection.
 */
void findMax(table_t *table, selection_t *selected)
{
	double max;
	char *endptr;
	bool found = false;
	selection_t newSelected;
	selectionCtor(&newSelected);

	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{	
			if (table->rows[i-1].columns[j-1].cellContent == NULL)
				continue;
			
			double tmp = strtod(table->rows[i-1].columns[j-1].cellContent, &endptr);
			if (*endptr != '\0')
				continue;
			
			if (!found)
			{
				max = tmp;
				found = true;
				selectionSet(&newSelected, i, j);
			}
			else if (max < tmp) 
			{
				max = tmp;
				selectionSet(&newSelected, i, j);
			}	
		}
	}

	if (found) changeCells(selected, &newSelected);
	free(newSelected.cells);
}

/**
 * Find the smallest number in the selected cells. If number is found, change
 * the selection.
 */
void findMin(table_t *table, selection_t *selected)
{
	double min;
	char *endptr;
	bool found = false;
	selection_t newSelected;
	selectionCtor(&newSelected);

	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{	
			if (table->rows[i-1].columns[j-1].cellContent == NULL)
				continue;
			
			double tmp = strtod(table->rows[i-1].columns[j-1].cellContent, &endptr);
			if (*endptr != '\0')
				continue;
			
			if (!found)
			{
				min = tmp;
				found = true;
				selectionSet(&newSelected, i, j);
			}
			else if (min > tmp) 
			{
				min = tmp;
				selectionSet(&newSelected, i, j);
			}	
		}
	}

	if (found) changeCells(selected, &newSelected);
	free(newSelected.cells);
}

/**
 * Change string to the position selected cells.
 */
int changeToCellPos(table_t *table, selection_t *selected, command_t *cmd)
{
	char tmp[MAX+1];
	int pos = 0;
	int commasCount = 0;
	bool containsSpecial = false;
	memset(tmp, '\0', MAX+1);
	selectionSet(selected, 0, 0);
	
	int len = strlen(cmd->parameters);
	for (int i = 1; i < len; i++)
	{
		if (cmd->parameters[i] == ',' || i == len-1)
		{	
			commasCount++;
			int number = atoi(tmp);
			
			if (tmp[0] == '-' && pos > 1) return -1;

			if (tmp[0] == '_' || tmp[0] == '-')
			{
				containsSpecial = true;
				if (commasCount == 1) 
				{
					number = 1;
					selected->cells[1].row = table->rowCount;
				}
				else if (commasCount == 2)
				{
					number = 1;
					selected->cells[1].col = table->maxCol;
				}
				else if (commasCount == 3) number = table->rowCount;
				else number = table->maxCol;
			} 			

			if (number < 1)
				return -1;

			if (commasCount == 1) selected->cells[0].row = number;
			else if (commasCount == 2) selected->cells[0].col = number;
			else if (commasCount == 3) selected->cells[1].row = number;
			else if (commasCount == 4) selected->cells[1].col = number;

			pos = 0;
			memset(tmp, '\0', MAX+1);
		}	
		else
		{
			tmp[pos] = cmd->parameters[i];
			pos++;
		}		
	}	
	if (commasCount == 2) 
	{
		if (!containsSpecial) 	
			selectionSet(selected, selected->cells[0].row, selected->cells[0].col);
		if (selected->cells[1].row == 0) 
			selected->cells[1].row = selected->cells[0].row;
		if (selected->cells[1].col == 0)
			selected->cells[1].col = selected->cells[0].col;	
	}
	
	if (selected->cells[0].row > selected->cells[1].row ||
		selected->cells[0].col > selected->cells[1].col)
		return -1;

	return 0;
}

/**
 * Change selection commands.
 */
int changeSelection(table_t *table, selection_t *selected, selection_t *temperory, command_t *cmd)
{
	if (strcmp(cmd->parameters, "[max]") == 0) findMax(table, selected);
	else if (strcmp(cmd->parameters, "[min]") == 0) findMin(table, selected);
	else if (strcmp(cmd->parameters, "[set]") == 0) 
		changeCells(temperory, selected);
	else if (strcmp(cmd->parameters, "[_]") == 0)
		changeCells(selected, temperory);
	else if (strstr(cmd->parameters, "[find") != 0) 
		findStr(table, selected, cmd);	
	else
		if(changeToCellPos(table, selected, cmd) == -1) 
		{
			fprintf(stderr, "Invalid given positions.\n");
			return -1;
		}

	if(selected->cells[1].row > table->rowCount || selected->cells[1].col > table->maxCol)  {
		tableRealloc(table, selected->cells[1].col, selected->cells[1].row);
		table->maxCol = selected->cells[1].col;
		tableRealloc(table, table->maxCol, table->rowCount);
	}

	return 0;
}

/**
 * Insert column before(side == 0) or after(side == 1) selected cells.
 */
int insertColumns(table_t *table, selection_t *selected, delim_t *del, int side)
{
	if (tableRealloc(table, table->maxCol+1, table->rowCount) == -1) return -1;
	table->maxCol++;
	
	int pos;
	if (side == 0)
		pos = selected->cells[0].col;
	else if (side == 1)
		pos = selected->cells[1].col+1;
	
	for (int i = table->maxCol-1; i >= pos; i--)
	{
		for (int j = 0; j < table->rowCount; j++)
		{
			if (table->rows[j].columns[i-1].cellContent != NULL)
				cellCset(&table->rows[j].columns[i], table->rows[j].columns[i-1].cellContent, del);
			cellDtor(&table->rows[j].columns[i-1]);
		}
	}	
	return 0;
}

/**
 * Delele all selected columns.
 */
int deleteColumns(table_t *table, selection_t *selected, delim_t *del)
{
	int count = selected->cells[1].col - selected->cells[0].col + 1;

	for (int i = selected->cells[0].col; i <= table->maxCol; i++)
	{
		for (int j = 0; j < table->rowCount; j++)
		{
			if (i == selected->cells[0].col) table->rows[j].colCount -= count;
			if (i <= selected->cells[1].col)
				cellDtor(&table->rows[j].columns[i-1]);

			if (i+count <= table->maxCol)
			{
				if (table->rows[j].columns[i-1+count].cellContent != NULL)
				{	
					cellCset(&table->rows[j].columns[i-1], table->rows[j].columns[i-1+count].cellContent, del);
					cellDtor(&table->rows[j].columns[i-1+count]);
				}
			}
			if (table->rows[j].colCount == 0) 
			{
				free(table->rows[j].columns);
				table->rowCount--;
			}
		}
	}
	
	if(table->rowCount == 0) 
		free(table->rows);
	else
		table->rows = realloc(table->rows, (table->rowCount) * sizeof(row_t));

	// Change selection if whole table is selected
	if (selected->cells[0].col == table->maxCol) selected->cells[0].col -= count;
	if (selected->cells[1].col == table->maxCol) selected->cells[1].col -= count;
	table->maxCol -= count;

	// if selection is beyond table dimensions
	if (selected->cells[1].col > table->maxCol)
	{
		tableRealloc(table, selected->cells[1].col, table->rowCount);
		table->maxCol = selected->cells[1].col;
	}

	return 0;
}
	
/**
 * Insert row above(side == 0) or below(side == 1) selected cells.
 */
int insertRows(table_t *table, selection_t *selected, delim_t *del, int side)
{
	if (tableRealloc(table, table->maxCol, table->rowCount+1) == -1) return -1;

	int pos;
	if (side == 0)
		pos = selected->cells[0].col;
	else if (side == 1)
		pos = selected->cells[1].col+1;
	
	for (int i = table->rowCount-1; i >= pos; i--)
	{
		for (int j = 0; j < table->maxCol; j++)
		{
			if (table->rows[i-1].columns[j].cellContent != NULL)
				cellCset(&table->rows[i].columns[j], table->rows[i-1].columns[j].cellContent, del);
			cellDtor(&table->rows[i-1].columns[j]);
		}
	}	
	return 0;
}

/**
 * Delete all selected rows.
 */
int deleteRows(table_t *table, selection_t *selected, delim_t *del)
{
	int count = selected->cells[1].row - selected->cells[0].row + 1;

	for (int i = selected->cells[0].row; i <= table->rowCount; i++)
	{
		for (int j = 0; j < table->maxCol; j++)
		{
			if (i <= selected->cells[1].row)
				cellDtor(&table->rows[i-1].columns[j]);

			if (i+count <= table->rowCount)
			{
				if (table->rows[i-1+count].columns[j].cellContent != NULL)
				{	
					cellCset(&table->rows[i-1].columns[j], table->rows[i-1+count].columns[j].cellContent, del);
					cellDtor(&table->rows[i-1+count].columns[j]);
				}
			}
		}
	}

	// Change selection if whole table is selected
	if (selected->cells[0].row == table->rowCount) selected->cells[0].row -= count;
	if (selected->cells[1].row == table->rowCount) selected->cells[1].row -= count;
	
	for (int i = table->rowCount-count; i < table->rowCount;i++)
		free(table->rows[i].columns); 
	table->rows = realloc(table->rows, (table->rowCount - count) * sizeof(row_t));
	table->rowCount -= count;

	//a if selection is beyond table dimensions
	if (selected->cells[1].row > table->rowCount) 
		tableRealloc(table, table->maxCol, selected->cells[1].row);
	return 0;
}

/**
 * Clear all selected cells.
 */
int clearCells(table_t *table, selection_t *selected)
{
	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
			cellDtor(&table->rows[i-1].columns[j-1]);
	}	
	return 0;
}

/**
 * Edit table commands.
 */
int editTable(table_t *table, selection_t *selected, command_t *cmd, delim_t *del)
{
	if (strcmp(cmd->parameters, "irow") == 0) insertRows(table, selected, del,  0);
	else if (strcmp(cmd->parameters, "arow") == 0) insertRows(table, selected, del,  1);
	else if (strcmp(cmd->parameters, "drow") == 0) deleteRows(table, selected, del);
	else if (strcmp(cmd->parameters, "icol") == 0) insertColumns(table, selected, del, 0);
	else if (strcmp(cmd->parameters, "acol") == 0) insertColumns(table, selected, del, 1);
	else if (strcmp(cmd->parameters, "dcol") == 0) deleteColumns(table, selected, del);
	else if (strcmp(cmd->parameters, "clear") == 0) clearCells(table, selected);
	else 
	{
		fprintf(stderr, "Invalid given argument(s).\n");
		return -1;
	}
	return 0;
}

/**
 * Increase the temperory var with index n by 1. If the temperory var us NULL
 * or contains string, change the content to 1.
 */
void incTemperoryVar(sequence_t *temperoryVar, char *c)
{
	int n = atoi(c);	
	char tmp[100];
	memset(tmp, '\0', 100);
	
	if (temperoryVar->array[n].parameters == NULL)
	{
		temperoryVar->array[n].parameters = malloc(2 * sizeof(char));
		temperoryVar->array[n].parameters[0] = '1';
		return;
	}
	float number = atof(temperoryVar->array[n].parameters);
	number += 1.0;
	sprintf(tmp, "%g", number);
		
	free(temperoryVar->array[n].parameters);
	temperoryVar->array[n].parameters = malloc( 100 * sizeof(char)); 

	strcpy(temperoryVar->array[n].parameters, tmp);
}

/**
 * Load the content of the temperory variable with index n and copy it to the
 * selected cells.
 */
void useTemperoryVar(table_t *table, selection_t *selected, sequence_t *temperoryVar, char *c, delim_t *del)
{
	int n = atoi(c);
	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{
			if (temperoryVar->array[n].parameters == NULL)
				cellDtor(&table->rows[i-1].columns[j-1]);
			else 
				cellCset(&table->rows[i-1].columns[j-1], temperoryVar->array[n].parameters, del);
		}
	}	
}

/**
 * Save the content of the last selected cell to the temperory variable with 
 * index n.
 */
void saveTemperoryVar(table_t *table, selection_t *selected, sequence_t *temperoryVar, char *c)
{
	int n = atoi(c);
	if (temperoryVar->array[n].parameters != NULL)
		free(temperoryVar->array[n].parameters);
	
	int row = selected->cells[1].row;
	int col = selected->cells[1].col;

	if(table->rows[row-1].columns[col-1].cellContent != NULL)
	{
		temperoryVar->array[n].parameters = malloc(table->rows[row-1].columns[col-1].cap * sizeof(char));
 		strcpy(temperoryVar->array[n].parameters, table->rows[row-1].columns[col-1].cellContent);
	}
}	

/**
 * Set the given number to the cell.
 */
void setValueToCell(cell_t *cell, delim_t *del, double number)
{
	if (cell->cellContent != NULL)
		cellDtor(cell);
	cellRealloc(cell, 100); 
	
	char tmp[100];
	memset(tmp, '\0', 100);
	sprintf(tmp, "%g", number);
	cellCset(cell, tmp, del);
}

/**
 * Change the given string to positions of the cell(s).
 */
int changeToPos(table_t *table, command_t *cmd, int *row, int *col)
{
	selection_t position;
	selectionCtor(&position);
	selectionSet(&position, 1, 1);
	changeToCellPos(table, &position, cmd);

	*row = position.cells[0].row;
	*col = position.cells[0].col;
	free(position.cells);

	if(*row > table->rowCount || *col > table->maxCol)
	{	
		fprintf(stderr, "Invalid column or row. Command will be ignored.\n");
		return -1;
	}

	return 0;
}

/**
 * Swap selected cells, cell by cell with given cell.
 */
void swapCells(table_t *table, selection_t *selected, command_t *cmd, delim_t *del)
{
	int row, col;
	if (changeToPos(table, cmd, &row, &col) == -1) {return;}
	
	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{
			int length = table->rows[i-1].columns[j-1].length;
			char tmp[length+1];
			memset(tmp, '\0', length+1);
		
			if (table->rows[i-1].columns[j-1].cellContent != NULL)
				sprintf(tmp, "%s", table->rows[i-1].columns[j-1].cellContent);
			if (table->rows[i-1].columns[j-1].cellContent != 0)
				cellDtor(&table->rows[i-1].columns[j-1]);
			if (table->rows[row-1].columns[col-1].cellContent != NULL)
				cellCset(&table->rows[i-1].columns[j-1], table->rows[row-1].columns[col-1].cellContent, del);
	
			cellDtor(&table->rows[row-1].columns[col-1]);
			if (strlen(tmp) != 0)
				cellCset(&table->rows[row-1].columns[col-1], tmp, del);
		}
	}
}

/**
 * mode == 0 => Calculate the sum of the numbers in the selected cells.
 * mode == 1 => Calculate the average of the numbers in the selected cells.
 */
void mathCells(table_t *table, selection_t *selected, command_t *cmd, delim_t *del, int mode)
{
	int row, col;
	if (changeToPos(table, cmd, &row, &col) == -1) {return;}
	
	int count = 0;
	char *endptr;
	double sum = 0.0;

	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{
			if (table->rows[i-1].columns[j-1].cellContent == NULL) continue;
			
			double tmp = strtod(table->rows[i-1].columns[j-1].cellContent, &endptr); 
			if (*endptr != '\0') continue;
	
			sum += tmp;
			count++;		
		}
	}
	if (mode == 1) sum = sum / count;
	setValueToCell(&table->rows[row-1].columns[col-1], del, sum);
}

/**
 * Count all empty cells in the selected cells. The result save to given cell. 
 */
void emptyCells(table_t *table, selection_t *selected, command_t *cmd, delim_t *del)
{
	int row, col;
	if (changeToPos(table, cmd, &row, &col) == -1) {return;}
	
	int emptyCount = 0;
	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{
			if (table->rows[i-1].columns[j-1].length == 0)
				emptyCount++;
		}
	}
	setValueToCell(&table->rows[row-1].columns[col-1], del, emptyCount);
} 

/**
 * The length of the last selected cell save to given cell. The last selected
 * cell is the cell at the bottom right.
 */
void len_cells(table_t *table, selection_t *selected, command_t *cmd, delim_t *del)
{
	int row, col;
	if(changeToPos(table, cmd, &row, &col) == -1 ) {return;}

	int length = table->rows[selected->cells[1].row-1].columns[selected->cells[1].col-1].length;
	
	setValueToCell(&table->rows[row-1].columns[col-1], del, length);
} 

/**
 * Set given value to selected cells.
 */
void setValue(table_t *table, selection_t *selected, delim_t *del, char *string)
{
	for (int i = selected->cells[0].row; i <= selected->cells[1].row; i++)
	{
		for (int j = selected->cells[0].col; j <= selected->cells[1].col; j++)
		{
			if (table->rows[i-1].columns[j-1].cellContent != NULL)
				cellDtor(&table->rows[i-1].columns[j-1]);
			if (string != NULL)
				cellCset(&table->rows[i-1].columns[j-1], string, del);
		}
	}	
}

/**
 * Delete empty columns at the end of the table.
 */
void deleteEmptyColumns(table_t *table, selection_t *selected, delim_t *del)
{
	if (table->rowCount == 0) return;
	for (int i = table->maxCol-1; i > 0; i--)
	{
		for (int j = 0; j < table->rowCount; j++)
		{
			if (table->rows[j].columns[i].length != 0 || table->rows[j].columns[i].cellContent != NULL)
				return;
		}
		selectionSet(selected, 1, i + 1);
		deleteColumns(table, selected, del);
	} 
}

/**
 * Change table according to given commands.
 */
int changeTable(table_t *table, sequence_t *commands, delim_t *del)
{
	selection_t selected;
	selectionCtor(&selected);
	// inicialization to [1,1]
	selectionSet(&selected, 1, 1);

	selection_t temperory;
	selectionCtor(&temperory);
	// inicialization to [0,0]
	selectionSet(&temperory, 0, 0);
	
	// temperory variables 0->9, containt NULL
	sequence_t temperoryVar;
	sequenceCtor(&temperoryVar); 
	sequenceRealloc(&temperoryVar, 1);
	sequenceRealloc(&temperoryVar, 10);	
	
	for (int i = 0; i < commands->length; i++)
	{
		if (strcmp(commands->array[i].name, "selection") == 0)
		{
			if (changeSelection(table, &selected, &temperory, &commands->array[i]) == -1) break;
		}
		else if (strcmp(commands->array[i].name, "editing") == 0)
			editTable(table, &selected, &commands->array[i], del); 
		else if (strcmp(commands->array[i].name, "def") == 0)
			saveTemperoryVar(table, &selected, &temperoryVar, &commands->array[i].parameters[1]);
		else if (strcmp(commands->array[i].name, "use") == 0)
			useTemperoryVar(table, &selected, &temperoryVar, &commands->array[i].parameters[1], del);
		else if (strcmp(commands->array[i].name, "inc") == 0)
			incTemperoryVar(&temperoryVar, &commands->array[i].parameters[1]);
		else if (strcmp(commands->array[i].name, "set") == 0)
			setValue(table, &selected, del, commands->array[i].parameters);
		else if (strcmp(commands->array[i].name, "swap") == 0)
			swapCells(table, &selected, &commands->array[i], del);
		else if (strcmp(commands->array[i].name, "sum") == 0)
			mathCells(table, &selected, &commands->array[i], del, 0);
		else if (strcmp(commands->array[i].name, "avg") == 0)
			mathCells(table, &selected, &commands->array[i], del, 1);
		else if (strcmp(commands->array[i].name, "count") == 0)
			emptyCells(table, &selected, &commands->array[i], del);
		else if (strcmp(commands->array[i].name, "len") == 0)
			len_cells(table, &selected, &commands->array[i], del);
		else {}
	}
	// Free memory
	free(selected.cells);
	free(temperory.cells);
	sequenceFree(&temperoryVar);
	// Delete empty
	deleteEmptyColumns(table, &selected, del);	

	return 0;
}

/********************/
/*					*/
/*		 main		*/
/*					*/
/********************/

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		fprintf(stderr, "No given arguments.\n");
		return 1;
	}

	// Delim inicialization
	delim_t del;
	delimInit(&del);	
	
	// Look for delim in arguments
	if (argc > 2)
		if(checkDelim(&del, argv[1], argv[2]) == -1)
		{
			delimDtor(&del);
			return 1;
		}
	
	// Check input file
	if (checkInputFile(argv[argc-1]) == -1)
	{
		delimDtor(&del);
		fprintf(stderr, "File does not exist in the current directory or fclose erorr.\n");
		return 1;
	}

	table_t table;
	if(tableInit(&table) == -1) 
	{
		delimDtor(&del);
		fprintf(stderr, "Malloc error.\n");
		return 1;
	}	

	// Load table from file
	if (loadTable(&table, argv[argc-1], &del) == -1)
	{
		fprintf(stderr, "Error ocurred while loading table.\n");
		tableFree(&table);
		delimDtor(&del);
		return 2;
	}
	if (tableRealloc(&table, table.maxCol, table.rowCount) == -1)
	{		
		fprintf(stderr, "Error ocurred while expanding table.\n");
		tableFree(&table);
		delimDtor(&del);
		return 2;
	}
	
	// Load commands from argument
	sequence_t commands;
	if (convertCommands(argv[argc-2], &commands) != -1)
	{
		// Change table
		changeTable(&table, &commands, &del);
	}
	// Print table
	printTable(&table, &del, argv[argc-1]);
	// Free memory
	tableFree(&table);
	delimDtor(&del);
	sequenceFree(&commands);
	
	return 0;
}