#include "filterproxymodel.h"

#include <algorithm>
#include <QDebug>

FilterProxyModel::FilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_filter = "";
}

void FilterProxyModel::setFilter(QString filter)
{
    m_filter = filter;
    invalidateFilter();
}

bool FilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex&) const
{
    if(m_filter.isEmpty()) return true;

    QModelIndex firstColumn = sourceModel()->index(sourceRow, 0);
    QModelIndex secondColumn = sourceModel()->index(sourceRow, 1);

    QString software = firstColumn.data().toString();
    QString selected = secondColumn.data().toString();
    QStringList options = secondColumn.data(Qt::UserRole).toStringList();

    if(software.contains(m_filter)) return true;
    if(selected.contains(m_filter)) return true;
    if(std::any_of(options.begin(), options.end(), [this](QString a) {
        return a.contains(m_filter);
    })) {
        return true;
    }
    return false;
}
