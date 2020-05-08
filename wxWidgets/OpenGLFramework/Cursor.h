/*! \file
	\brief Plik nag��wkowy dla klasy CCursor.
*/
#pragma once

/// Definicja sta�ej maksymalnej liczby kursor�w.
#define MAX_CURSOR 3

/// Klasa zarz�dzaj�ca kursorami.
class CCursor  
{
public:
	/// Konstruktor.
	/** Konstruktor. */
	CCursor();

	/// Destruktor.
	/** Destruktor. */
	virtual ~CCursor();

	/// Metoda ustawiaj�ca kursor.
	/** \param[in] iIndex Indeks kursora, kt�ry chcemy uaktywni�. */
	GLvoid SetMyCursor(GLint iIndex);

	/// Metoda zwracaj�ca indeks aktualnie ustawionego kursora.
	/** \return Liczba ca�kowita zwracaj�ca indeks aktualnie ustawionego ursora. */
	inline GLint GetMyCursor()	{ return m_iCursor; }

private:
	HCURSOR m_hCursor[MAX_CURSOR];	///< Tablica przechowuj�ca wczytane kursory.

	/// Zmienna pami�taj�ca indeks ostatnio ustawionego kursora.
	GLint m_iCursor;
};