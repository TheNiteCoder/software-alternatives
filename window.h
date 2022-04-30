#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QProcess>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QLineEdit>

#include "filterproxymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const {
        auto selections = index.data(Qt::UserRole).value<QStringList>();
        auto current = index.data(Qt::DisplayRole).value<QString>();
        QComboBox* comboBox = new QComboBox{parent};
        comboBox->addItems(selections);
        comboBox->setCurrentText(current);
        return comboBox;
    }
    virtual void deleteEditor(QWidget* editor, const QModelIndex&) {
        editor->deleteLater();
    }
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        comboBox->clear();
        comboBox->addItems(index.data(Qt::UserRole).value<QStringList>());
        comboBox->setCurrentText(index.data(Qt::DisplayRole).value<QString>());
    }
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        model->setData(index, comboBox->currentText(), Qt::DisplayRole);
    }
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const {
        editor->setGeometry(option.rect);
    }
    virtual QString displayText(const QVariant& value, const QLocale&) const {
        return value.toString();
    }
};


class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

    void loadModel();
    void save();
    void resizeEvent(QResizeEvent* event);
    void moveEvent(QMoveEvent* event);
private slots:
    void adjustPopupFilter();
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void nextSave();
protected:
    bool eventFilter(QObject* obj, QEvent* event);
private:
    QList<QPair<QString, QString>> m_changes;
    QProcess m_process{this};
    QStandardItemModel m_model;
    FilterProxyModel m_filterModel;
    QLineEdit m_lineEdit_filter;

    Ui::Window *ui;
};
#endif // WINDOW_H
