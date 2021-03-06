/*! \file
	\brief Plik nagłówkowy dla klasy CCursor.
*/
#pragma once

/// Definicja stałej maksymalnej liczby kursorów.
#define MAX_CURSOR 3

/// Klasa zarządzająca kursorami.
class CCursor  
{
public:
	/// Konstruktor.
	/** Konstruktor. */
	CCursor();

	/// Destruktor.
	/** Destruktor. */
	virtual ~CCursor();

	/// Metoda ustawiająca kursor.
	/** \param[in] iIndex Indeks kursora, który chcemy uaktywnić. */
	GLvoid SetMyCursor(GLint iIndex);

	/// Metoda zwracająca indeks aktualnie ustawionego kursora.
	/** \return Liczba całkowita zwracająca indeks aktualnie ustawionego ursora. */
	inline GLint GetMyCursor()	{ return m_iCursor; }

private:
	HCURSOR m_hCursor[MAX_CURSOR];	///< Tablica przechowująca wczytane kursory.

	/// Zmienna pamiętająca indeks ostatnio ustawionego kursora.
	GLint m_iCursor;
};