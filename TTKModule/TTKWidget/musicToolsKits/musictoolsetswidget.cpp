#include "musictoolsetswidget.h"
#include "ui_musictoolsetswidget.h"
#include "musicaudiorecorderwidget.h"
#include "musictimerwidget.h"
#include "musiclocalsongsmanagerwidget.h"
#include "musictransformwidget.h"
#include "musicdesktopwallpaperwidget.h"
#include "musicnetworkconnectiontestwidget.h"
#include "musicvolumegainwidget.h"
#include "musicsongdlnatransferwidget.h"
#include "musicsongringtonemakerwidget.h"
#include "musicmessagebox.h"
#include "musicapplication.h"
#include "musicrightareawidget.h"
#include "musicmessagebox.h"
#include "musicspectrumwidget.h"
#include "musicsinglemanager.h"

MusicToolSetsWidget::MusicToolSetsWidget(QWidget *parent)
    : MusicAbstractMoveWidget(parent),
      m_ui(new Ui::MusicToolSetsWidget)
{
    m_ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_QuitOnClose, true);

    m_ui->topTitleCloseButton->setIcon(QIcon(":/functions/btn_close_hover"));
    m_ui->topTitleCloseButton->setStyleSheet(MusicUIObject::MQSSToolButtonStyle04);
    m_ui->topTitleCloseButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->topTitleCloseButton->setToolTip(tr("Close"));
    connect(m_ui->topTitleCloseButton, SIGNAL(clicked()), SLOT(close()));

    m_ui->listItemWidget->setFrameShape(QFrame::NoFrame);
    m_ui->listItemWidget->verticalScrollBar()->setStyleSheet(MusicUIObject::MQSSScrollBarStyle01);

    m_ui->listItemWidget->setIconSize(QSize(60, 60));
    m_ui->listItemWidget->setViewMode(QListView::IconMode);
    m_ui->listItemWidget->setMovement(QListView::Static);

    m_ui->listItemWidget->setSpacing(11);
    addListWidgetItem();

    connect(m_ui->listItemWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(itemHasClicked(QListWidgetItem*)));
}

MusicToolSetsWidget::~MusicToolSetsWidget()
{
    M_SINGLE_MANAGER_PTR->removeObject(getClassName());
    clearAllItems();
    delete m_ui;
}

void MusicToolSetsWidget::addListWidgetItem()
{
    typedef struct ItemPair
    {
        QString m_icon;
        QString m_name;

        ItemPair() { }
        ItemPair(const QString &icon, const QString &name)
        {
            m_icon = icon;
            m_name = name;
        }
    }ItemPair;
    TTK_DECLARE_LISTS(ItemPair);

    ItemPairs pairs;
    pairs << ItemPair(":/tools/lb_localmanager", tr("localmanager"))
          << ItemPair(":/tools/lb_recorder", tr("recorder"))
          << ItemPair(":/tools/lb_bell", tr("bell"))
          << ItemPair(":/tools/lb_timer", tr("timer"))
          << ItemPair(":/tools/lb_transform", tr("transform"))
          << ItemPair(":/tools/lb_spectrum", tr("spectrum"))
          << ItemPair(":/tools/lb_wallpaper", tr("wallpaper"))
          << ItemPair(":/tools/lb_connections" ,tr("connections"))
          << ItemPair(":/tools/lb_gain", tr("gain"))
          << ItemPair(":/tools/lb_dlna", tr("dlna"))
          << ItemPair(":/tools/lb_detect", tr("detect"))
          << ItemPair(":/tools/lb_screen_saver", tr("saver"));

    foreach(const ItemPair &pair, pairs)
    {
        QListWidgetItem *item = new QListWidgetItem(QIcon(pair.m_icon), pair.m_name, m_ui->listItemWidget);
#if TTK_QT_VERSION_CHECK(5,13,0)
        item->setForeground(QColor(MusicUIObject::MQSSColorStyle12_S));
#else
        item->setTextColor(QColor(MusicUIObject::MQSSColorStyle12_S));
#endif
        item->setSizeHint(QSize(80, 90));
        m_ui->listItemWidget->addItem(item);
    }
}

void MusicToolSetsWidget::itemHasClicked(QListWidgetItem *item)
{
    switch( m_ui->listItemWidget->row(item) )
    {
        case 0:
            {
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicLocalSongsManagerWidget);
                break;
            }
        case 1:
            {
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicAudioRecorderWidget);
                break;
            }
        case 2:
            {
                MusicSongRingtoneMaker(this).exec();
                break;
            }
        case 3:
            {
                MusicTimerWidget timer(this);
                QStringList songlist;
                MusicApplication::instance()->getCurrentPlaylist(songlist);
                timer.setSongStringList(songlist);
                timer.exec();
                break;
            }
        case 4:
            {
                MusicTransformWidget(this).exec();
                break;
            }
        case 5:
            {
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicSpectrumWidget);
                break;
            }
        case 6:
            {
#ifdef Q_OS_WIN
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicDesktopWallpaperWidget);
#else
                MusicMessageBox message;
                message.setText(tr("Not Supported On Current Plantform!"));
                message.exec();
#endif
                break;
            }
        case 7:
            {
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicNetworkConnectionTestWidget);
                break;
            }
        case 8:
            {
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicVolumeGainWidget);
                break;
            }
        case 9:
            {
                M_SINGLE_MANAGER_WIDGET_CLASS(MusicSongDlnaTransferWidget);
                break;
            }
        case 10:
            {
                MusicRightAreaWidget::instance()->musicFunctionClicked(MusicRightAreaWidget::IndentifyWidget);
                break;
            }
        case 11:
            {
                MusicRightAreaWidget::instance()->musicFunctionClicked(MusicRightAreaWidget::ScreenSaverWidget);
                break;
            }
        default:
            break;
    }
}

void MusicToolSetsWidget::show()
{
    setBackgroundPixmap(m_ui->background, size());
    MusicAbstractMoveWidget::show();
}

void MusicToolSetsWidget::clearAllItems()
{
    m_ui->listItemWidget->clear();
}

void MusicToolSetsWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    QListWidgetItem *it = m_ui->listItemWidget->currentItem();
    if(it)
    {
        itemHasClicked(it);
    }
}
