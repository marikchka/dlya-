#include <QtWidgets>
#include <cstdlib>
#include <ctime>
#include <QMediaPlayer>
#include <QSoundEffect>

class CustomGraphicsView : public QGraphicsView
{

public:
    CustomGraphicsView(QGraphicsScene* scene) : QGraphicsView(scene), pixMapItemBonus(new QGraphicsPixmapItem())
    {
        soundEffect = new QSoundEffect;
        soundEffect->setSource(QUrl::fromLocalFile("C:\\Users\\ssson\\Downloads\\David Bowie - Starman (2012 Remastered Version).mp3"));
        soundEffect->setLoopCount(QSoundEffect::Infinite);
        soundEffect->setVolume(50);


        soundEffect->play();


        pixMapItemWolf.setPixmap(QPixmap("C:\\Users\\ssson\\Downloads\\wolfa.png"));
        pixMapItemWolf.setPos((scene->width() - pixMapItemWolf.pixmap().width()) / 2,
                              scene->height() - pixMapItemWolf.pixmap().height() + 250);
        pixMapItemWolf.setFlags(QGraphicsItem::ItemIsMovable);
        pixMapItemWolf.setZValue(0);
        scene->addItem(&pixMapItemWolf);


        scoreTextItem = new QGraphicsTextItem("Score: 0");
        scoreTextItem->setDefaultTextColor(Qt::white);
        scene->addItem(scoreTextItem);
        scoreTextItem->setPos(pixMapItemWolf.pos() + QPointF(300, 60));


        QFont font = scoreTextItem->font();
        font.setPointSize(font.pointSize() * 2);
        scoreTextItem->setFont(font);

        totalRockets = 35;
        rocketsLaunched = 0;
        bonusCaught = 0;
        bonusTotal = 15;

        QPixmap backgroundImage("C:\\Users\\ssson\\Downloads\\suc4Nm.jpeg");


        qreal widthRatio = (scene->width() + 400) / static_cast<qreal>(backgroundImage.width());
        qreal heightRatio = (scene->height() + 300) / static_cast<qreal>(backgroundImage.height());
        QPixmap scaledBackgroundImage = backgroundImage.scaled(scene->width() + 400, scene->height() + 300);

        backgroundItem = new QGraphicsPixmapItem(scaledBackgroundImage);
        backgroundItem->setZValue(-1);
        scene->addItem(backgroundItem);


        backgroundItem->setPos(scene->sceneRect().topLeft());


        scoreRequirementTextItem = new QGraphicsTextItem("Score 16. Don't catch 3 black rockets");
        scoreRequirementTextItem->setDefaultTextColor(Qt::white);
        QFont scoreFont = scoreRequirementTextItem->font();
        scoreFont.setPointSize(20);
        scoreRequirementTextItem->setFont(scoreFont);
        scene->addItem(scoreRequirementTextItem);
        scoreRequirementTextItem->setPos(scene->width() / 2 - 80, scene->height() / 2 - 100);
        playGameButton = new QPushButton("Play Game", this);
        playGameButton->setGeometry(QRect(scene->width() / 2 - 75, scene->height() / 2, 200, 60));
        connect(playGameButton, &QPushButton::clicked, this, &CustomGraphicsView::startGame);


        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CustomGraphicsView::update);
        timer->start(16);
    }

protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        switch (event->key()) {
        case Qt::Key_Left:
            pixMapItemWolf.setPos(pixMapItemWolf.pos() - QPointF(18, 0));
            break;
        case Qt::Key_Right:
            pixMapItemWolf.setPos(pixMapItemWolf.pos() + QPointF(18, 0));
            break;
        default:
            break;
        }
    }

private slots:
    void update()
    {
        qreal fallSpeed = 1.5;


        for (auto it = rockets.begin(); it != rockets.end(); ) {
            (*it)->setPos((*it)->pos() + QPointF(0, fallSpeed));


            if ((*it)->collidesWithItem(&pixMapItemWolf)) {

                scene()->removeItem(*it);
                delete *it;
                it = rockets.erase(it);

                score++;
                updateScoreText();
            } else if ((*it)->y() > scene()->height() + 100) {

                scene()->removeItem(*it);
                delete *it;
                it = rockets.erase(it);
            } else {
                ++it;
            }
        }


        if (!bonusItems.isEmpty()) {
            bonusItems.first()->setPos(bonusItems.first()->pos() + QPointF(0, fallSpeed));


            if (bonusItems.first()->collidesWithItem(&pixMapItemWolf)) {

                scene()->removeItem(bonusItems.first());
                delete bonusItems.first();
                bonusItems.removeFirst();

                bonusCaught++;
                if (bonusCaught == 3) {
                    QMessageBox::information(this, "You Lose", "Sorry! You lose!");
                    qApp->quit();
                }
            } else if (bonusItems.first()->y() > scene()->height() + 100) {

                scene()->removeItem(bonusItems.first());
                delete bonusItems.first();
                bonusItems.removeFirst();
            }
        }


        if (rocketsLaunched == totalRockets) {
            if (score >= 16) {
                QMessageBox::information(this, "You Win", "Congratulations! You win!");
            } else {
                QMessageBox::information(this, "You Lose", "Sorry! You lose!");
            }
            qApp->quit();


        }
    }

    void updateScoreText()
    {
        scoreTextItem->setPlainText("Score: " + QString::number(score));
        scoreTextItem->setDefaultTextColor(Qt::white);
    }

    void createRockets()
    {
        createRocketTimer = new QTimer(this);
        connect(createRocketTimer, &QTimer::timeout, this, &CustomGraphicsView::createRocket);
        createRocketTimer->start(2000);
    }

    void createBonus()
    {


        for (int i = 0; i < bonusTotal; ++i) {
            QGraphicsPixmapItem* bonus = new QGraphicsPixmapItem();
            bonus->setPixmap(QPixmap("C:\\Users\\ssson\\Downloads\\bayraktar.png"));
            bonus->setZValue(1);
            scene()->addItem(bonus);

            qreal bonusX = static_cast<qreal>(std::rand()) / RAND_MAX * (scene()->width() - 100) + 50;
            bonus->setPos(bonusX, -30);

            bonusItems.push_back(bonus);
        }
    }

    void createRocket()
    {
        if (rocketsLaunched < totalRockets) {
            QGraphicsPixmapItem* rocket = new QGraphicsPixmapItem();
            rocket->setPixmap(QPixmap("C:\\Users\\ssson\\Downloads\\rocket.png"));
            rocket->setZValue(1);
            scene()->addItem(rocket);

            qreal minX = -100.0;
            qreal maxX = scene()->width() + 100.0;

            qreal initialX = static_cast<qreal>(std::rand()) / RAND_MAX * (maxX - minX) + minX;

            rocket->setPos(initialX, -30);

            rockets.push_back(rocket);
            ++rocketsLaunched;
        } else {
            createRocketTimer->stop();
        }
    }

    void startGame()
    {

        playGameButton->hide();
        scoreRequirementTextItem->hide();


        QTimer::singleShot(2000, this, &CustomGraphicsView::createRockets);


        QTimer::singleShot(2000, this, &CustomGraphicsView::createBonus);
    }

private:
    QGraphicsPixmapItem pixMapItemWolf;
    QVector<QGraphicsPixmapItem*> rockets;
    QVector<QGraphicsPixmapItem*> bonusItems;
    QGraphicsTextItem* scoreTextItem;
    QGraphicsTextItem* scoreRequirementTextItem;
    int score = 0;

    QSoundEffect* soundEffect;
    QTimer* createRocketTimer;
    int totalRockets;
    int rocketsLaunched;

    QGraphicsPixmapItem* pixMapItemBonus;
    int bonusCaught;
    int bonusTotal;

    QGraphicsPixmapItem* backgroundItem;
    QPushButton* playGameButton;
};

int main(int argc, char** argv)
{
    std::srand(static_cast<unsigned>(std::time(0)));

    QApplication app(argc, argv);

    QGraphicsScene scene(QRectF(-340, 300, 1480, 800));

    CustomGraphicsView view(&scene);
    view.show();

    return app.exec();
}
