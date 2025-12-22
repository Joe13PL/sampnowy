/*
 * =============================================================================
 *  DATABASE QUERIES - Zapytania SQL
 * =============================================================================
 *  
 *  Modu??: database/queries.inc
 *  Opis: Zawiera zapytania SQL i funkcje pomocnicze do operacji bazodanowych
 *
 *  UWAGA: G????wne funkcje MySQL s?? w mysql_connection.inc
 *         Ten plik zawiera tylko dodatkowe helpery
 *
 * =============================================================================
 */

#if defined _queries_included
    #endinput
#endif
#define _queries_included

// ===========================================================================
// MAKRA ZAPYTA??
// ===========================================================================

// Maksymalna d??ugo???? zapytania (je??li nie zdefiniowana)
#if !defined MAX_QUERY_LENGTH
    #define MAX_QUERY_LENGTH            2048
#endif

// ===========================================================================
// EOF
// ===========================================================================
// ===========================================================================

