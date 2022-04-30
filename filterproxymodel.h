#ifndef FILTERPROXYMODEL_H
#define FILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilterProxyModel(QObject* parent = nullptr);
public slots:
    void setFilter(QString filter);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex&) const override;
private:
    QString m_filter;
};

#endif // FILTERPROXYMODEL_H
