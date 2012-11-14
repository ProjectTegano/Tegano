//
// WolframeWidgets.cpp
//

#include "WolframeWidgets.hpp"

#include "FileChooserPlugin.hpp"
#include "FormWidgetPlugin.hpp"

#include <QtPlugin>

WolframeWidgets::WolframeWidgets( QObject *_parent )
	: QObject( _parent )
{
	m_plugins.append( new FileChooserPlugin( this ) );
	m_plugins.append( new FormWidgetPlugin( this ) );
}

QList<QDesignerCustomWidgetInterface *> WolframeWidgets::customWidgets( ) const
{
	return m_plugins;
}

Q_EXPORT_PLUGIN2( wolframewidgets, WolframeWidgets )
