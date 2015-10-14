#include "editor.h"
#include "graphicelement.h"
#include "qneconnection.h"
#include "serializationfunctions.h"
#include <QApplication>

void SerializationFunctions::serialize( const QList< QGraphicsItem* > &items, QDataStream &ds ) {
  foreach( QGraphicsItem * item, items ) {
    if( item->type( ) == GraphicElement::Type ) {
      ds << item->type( );
      ds << ( quint64 ) ( ( GraphicElement* ) item )->elementType( );
      ( ( GraphicElement* ) item )->save( ds );
    }
  }
  foreach( QGraphicsItem * item, items ) {
    if( item->type( ) == QNEConnection::Type ) {
      ds << item->type( );
      ( ( QNEConnection* ) item )->save( ds );
    }
  }
}

QList< QGraphicsItem* > SerializationFunctions::deserialize( Editor *editor, QDataStream &ds ) {
  Scene *scene = editor->getScene( );

  QList< QGraphicsItem* > itemList;
  QMap< quint64, QNEPort* > portMap;
  double version = QApplication::applicationVersion( ).toDouble( );
  while( !ds.atEnd( ) ) {
    int type;
    ds >> type;
    if( type == GraphicElement::Type ) {
      quint64 elmType;
      ds >> elmType;
      GraphicElement *elm = editor->getFactory( ).buildElement( ( ElementType ) elmType );
      if( elm ) {
        scene->addItem( elm );
        itemList.append( elm );
        elm->load( ds, portMap, version );
        elm->setPos( ( elm->pos( ) ) );
        elm->setSelected( true );
      }
      else {
        throw( std::runtime_error( "Could not build element." ) );
      }
    }
    else if( type == QNEConnection::Type ) {
      QNEConnection *conn = new QNEConnection( 0 );
      scene->addItem( conn );
      conn->setSelected( true );
      if( !conn->load( ds, portMap ) ) {
        scene->removeItem( conn );
        delete conn;
      }
      else {
        itemList.append( conn );
      }
    }
    else {
      throw( std::runtime_error( "Invalid element type. Data is possibly corrupted." ) );
    }
  }
  return( itemList );
}