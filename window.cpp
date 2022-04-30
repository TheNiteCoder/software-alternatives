#include "window.h"
#include "ui_window.h"

#include <QDebug>
#include <QRegularExpression>



Window::Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Window)
{
    m_filterModel.setSourceModel(&m_model);
    ui->setupUi(this);
    setWindowTitle(tr("Software Alternatives"));
    ui->tableView_Softwares->setModel(&m_filterModel);
    ui->tableView_Softwares->setItemDelegateForColumn(1, new ComboBoxDelegate);
    ui->tableView_Softwares->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->tableView_Softwares->setSelectionMode(QTableView::NoSelection);

//    ui->pushButton_filter->setCheckable(true);

    m_lineEdit_filter.setParent(this);
    m_lineEdit_filter.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);
    m_lineEdit_filter.installEventFilter(this);

    connect(ui->pushButton_filter, &QPushButton::clicked, [=](){
        m_lineEdit_filter.setVisible(!m_lineEdit_filter.isVisible());
        if(!m_lineEdit_filter.isVisible()) {
            m_filterModel.setFilter("");
            m_filterModel.invalidate();
        }
        else {
            m_filterModel.setFilter(m_lineEdit_filter.text());
            m_filterModel.invalidate();
        }
    });

    connect(ui->pushButton_apply, &QPushButton::clicked, [=](){
        save();
    });

    connect(&m_lineEdit_filter, &QLineEdit::textChanged, &m_filterModel, &FilterProxyModel::setFilter);

    connect(ui->pushButton_close, &QPushButton::clicked, this, &QWidget::close);

    connect(&m_model, &QAbstractItemModel::dataChanged, this, &Window::onDataChanged);
    loadModel();

    ui->tableView_Softwares->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView_Softwares->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    adjustPopupFilter();
}

Window::~Window()
{
    delete ui;
}

void Window::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustPopupFilter();
}

void Window::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    adjustPopupFilter();
}

void Window::adjustPopupFilter()
{
    QRect rect = ui->pushButton_filter->geometry();
    QPoint bottomLeft = mapToGlobal(rect.bottomLeft());
    m_lineEdit_filter.setGeometry(QRect(bottomLeft, QSize(rect.width() * 3, 40)));
}

void Window::loadModel()
{
    disconnect(&m_model, &QAbstractItemModel::dataChanged, this, &Window::onDataChanged);
    m_changes.clear();
    m_model.clear();
    m_model.setHorizontalHeaderLabels({tr("Name"), tr("Options")});
    m_process.start("update-alternatives", {"--get-selections"});
    m_process.waitForFinished();
    QString output = m_process.readAll();
    QStringList lines = output.split("\n");
    for(auto line : lines) {
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if(parts.length() < 3) {
            continue;
        }
        QString name = parts[0];
        QString current = parts[2];
        m_process.start("update-alternatives", {"--list", name});
        m_process.waitForFinished();
        QString out = m_process.readAll();
        QStringList options = out.split("\n");
        options.removeLast();
        auto item = new QStandardItem(current);
        item->setData(options, Qt::UserRole);
        auto item2 = new QStandardItem(parts[0]);
        item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_model.appendRow({item2, item});
    }
    connect(&m_model, &QAbstractItemModel::dataChanged, this, &Window::onDataChanged);
}

void Window::save()
{
    for(auto change : m_changes) {
        m_process.start("update-alternatives", {"--set", change.first, change.second});
        m_process.waitForFinished();
    }
    m_changes.clear();
}

void Window::onDataChanged(const QModelIndex &topLeft, const QModelIndex&, const QVector<int>&)
{
    QString software = m_model.data(m_model.index(topLeft.row(), 0)).toString();
    QString choice = topLeft.data().toString();
    m_changes.append({software, choice});
}

void Window::nextSave()
{
    auto change = m_changes.takeFirst();
}

bool Window::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FocusOut)
    {
        ui->pushButton_filter->setChecked(false);
        return false;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}

