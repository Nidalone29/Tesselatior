#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"

/**
        Funzione che trasforma un angolo da gradi in radianti
        @param deg angolo in gradi
        @return l'angolo in radianti
*/
float to_radiant(float deg);

/**
        Funzione che trasforma un angolo da radianti in gradi
        @param deg angolo in radianti
        @return l'angolo in gradi
*/
float to_degree(float rad);

/**
        Classe che incapsula diverse funzioni per calcolare le matrici di
        trasformazione geometrica e comporre una unica matrice di
        trasformazione.
        le chiamate alle funzioni rotate, translate e scale, applicano la
   trasformazione richiesta alla matrice composita corrente: composita = T *
   composita. Chiamate successive alle funzioni concatenano le trasformazioni.

        le funzioni statiche rotation, rotation3, translation, scaling
   permettono di calcolare e ottenere in output le matrici di trasformazioni
   richieste. Possono essere usate senza istanziare la classe ma come:
   LocalTransform::rotation(...)
*/
class LocalTransform {
 public:
  /**
          Costruttore. Inizializza la matrice composità alla matrice identità.
  */
  LocalTransform();

  /**
          Applica la rotazione alla matrice corrente. Gli angoli sono in gradi.
          @param degX angolo di rotazione rispetto all'asse X
          @param degY angolo di rotazione rispetto all'asse Y
          @param degZ angolo di rotazione rispetto all'asse Z
  */
  void rotate(float degX, float degY, float degZ);

  /**
          Applica la rotazione alla matrice corrente. Gli angoli sono in gradi.
          @param angles vettore che contiene i 3 angoli di rotazione
  */
  void rotate(const glm::vec3& angles);

  /**
          Applica la traslazione alla matrice corrente.
          @param x offset di traslazione rispetto all'asse X
          @param y offset di traslazione rispetto all'asse Y
          @param z offset di traslazione rispetto all'asse Z
  */
  void translate(float x, float y, float z);

  /**
          Applica la traslazione alla matrice corrente.
          @param offset vettore che contiene gli offset di traslazione
  */
  void translate(const glm::vec3& offset);

  /**
          Applica lo scaling alla matrice corrente.
          @param sx fattore di scaling rispetto all'asse X
          @param sy fattore di scaling rispetto all'asse Y
          @param sz fattore di scaling rispetto all'asse Z
  */
  void scale(float sx, float sy, float sz);

  /**
          Applica lo scaling uniforme alla matrice corrente.
          @param sc valore di scaling
  */
  void scale(float sc);

  /**
          Applica lo scaling alla matrice corrente.
          @param factor vettore con i tre valori di scaling
  */
  void scale(const glm::vec3& factor);

  /**
          Ritorna la matrice di trasformazione composita.
          @return la matrice di trasformazione composita.
  */
  const glm::mat4& T() const;

  /**
          Inizializza la matrice composita alla matrice identità.
  */
  void reset();

  /**
          Funzione statica che calcola la matrice di rotazione dati tre angoli
          in gradi.

          @param degX angolo di rotazione rispetto all'asse X
          @param degY angolo di rotazione rispetto all'asse Y
          @param degZ angolo di rotazione rispetto all'asse Z
          @return la matrice 4x4 di rotazione
  */
  static glm::mat4 rotation(float degX, float degY, float degZ);

  /**
          Funzione statica che calcola la matrice di rotazione intorno ad
          un vettore di un dato angolo (in gradi).
          La trasfromazione unsa la formula di Rodriguez.

          @param deg angolo di rotazione in gradi
          @param axis vettore che contiene l'asse di rotazione
          @return la matrice 3x3 di rotazione
  */
  static glm::mat3 rotation3(float deg, const glm::vec3& axis);

  /**
          Funzione statica che calcola la matrice di traslazione sui tre assi.

          @param x offsetX di traslazione rispetto all'asse X
          @param y offsetY di traslazione rispetto all'asse Y
          @param z offsetZ di traslazione rispetto all'asse Z
          @return la matrice 4x4 di traslazione
  */
  static glm::mat4 translation(float offsetX, float offsetY, float offsetZ);

  /**
          Funzione statica che calcola la matrice di scaling sui tre assi.

          @param factorX fattore di scaling rispetto all'asse X
          @param factorY fattore di scaling rispetto all'asse Y
          @param factorZ fattore di scaling rispetto all'asse Z
          @return la matrice 4x4 di scaling
  */
  static glm::mat4 scaling(float factorX, float factorY, float factorZ);

 private:
  glm::mat4 _combined;  ///<< matrice composita

  glm::mat4 _rotation;     ///<< matrice di rotazione
  glm::mat4 _scaling;      ///<< matrice di scaling
  glm::mat4 _translation;  ///<< matrice di traslazione

  /**
          Funzione che aggiorna la matrice composita
  */
  void update();
};

#endif