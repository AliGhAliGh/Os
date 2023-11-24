#ifndef DATA_H
#define DATA_H

typedef struct _ingredient
{
    char *name;
    int count;
} Ingredient;

typedef struct _ingredients
{
    Ingredient *ingredients;
    int count;
} Ingredients;

typedef struct _recipe
{
    Ingredients ingredients;
    char *name;
} Recipe;

typedef struct _recipes
{
    Recipe *recipes;
    int count;
} Recipes;

#endif