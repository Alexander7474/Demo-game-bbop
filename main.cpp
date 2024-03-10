////////////////////////////////////////////////////////////////////
//CECI N'EST QU'UN EXEMPLE POUR TESTER SI BBOP EST BIEN INSTALLER
////////////////////////////////////////////////////////////////////
#include <BBOP/Graphics/bbopMathClass.h>
#include <BBOP/Graphics/shapeClass.h>
#include <BBOP/Graphics/textureClass.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <BBOP/Graphics.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>
#include <irrklang/irrKlang.h>

using namespace std;
using namespace irrklang;

int main() {
  GLFWwindow * window;
  bbopInit(1143,1404,"test",window);
  
  Scene defaultScene(1.0f,Vector3i(255,200,200));

  ISoundEngine *SoundEngine = createIrrKlangDevice();
  SoundEngine->addSoundSourceFromFile("sound/gun-shoot_0.wav", ESM_AUTO_DETECT, true);
  SoundEngine->addSoundSourceFromFile("sound/gun-reload.wav", ESM_AUTO_DETECT, true);

  char file[50];
  vector<Texture> textureList;
  int spriteAnimState = 0;
  for(int i = 0; i < 20; i++){
    sprintf(file, "img/feet/run/survivor-run_%d.png", i);
    textureList.push_back(Texture(file));
  }
  Sprite player(textureList[spriteAnimState]);
  player.setPosition(Vector2f(BBOP_WINDOW_SIZE.x/2.0f,BBOP_WINDOW_SIZE.y/2.0f));
  player.setSize(Vector2f(50.0f,50.0f));
  player.setOrigin(Vector2f(25.0f,25.0f));
  Camera defaultCam(player.getPosition(), 0.6f);
 
  vector<Texture> textureListGun;
  int spriteAnimStateGun = 0;
  for(int i = 0; i < 3; i++){
    sprintf(file, "img/handgun/shoot/survivor-shoot_handgun_%d.png", i);
    textureListGun.push_back(Texture(file));
  }
  Sprite playerGun(textureListGun[spriteAnimStateGun]);
  playerGun.setPosition(player.getPosition());
  playerGun.setOrigin(Vector2f(32.5f,32.5f));
  playerGun.setSize(Vector2f(65.0f,65.0f));

  CircleShape test(3, 100.0f);
  test.setColor(Vector3i(300,150,150));
  test.setSize(Vector2f(1.5f,1.0f));
  test.setOrigin(Vector2f(300.0f,100.0f));
  test.setAlpha(0.5);

  RectangleShape box;
  box.setPosition(Vector2f(150.0f,150.0f));
  box.setSize(Vector2f(300.0f,300.0f));
  box.setColor(Vector3i(255,150,200));

  vector<RectangleShape> bulletList;
  vector<Vector2f> bulletMove;
  Vector2f bulletSpeed(50.0f,50.0f);

  double timefromLastShoot = glfwGetTime();
  double timefromReload = glfwGetTime();

  // Main while loop
	while (!glfwWindowShouldClose(window))
	{
    bbopCleanWindow(window, Vector3i(200,200,200),1.0);

    //////////////////////////////////////////////////////////////
    ///code zone
    //////////////////////////////////////////////////////////////

    if(spriteAnimState>19)
      spriteAnimState = 0;
    player.setTexture(textureList[spriteAnimState]);
   
    if(spriteAnimStateGun > 0)
      spriteAnimStateGun ++;
    if(spriteAnimStateGun>2)
      spriteAnimStateGun = 0;
    playerGun.setTexture(textureListGun[spriteAnimStateGun]);

    defaultScene.Use();
    defaultScene.useCamera(&defaultCam);
    defaultScene.Draw(box);
    defaultScene.Draw(player);
    defaultScene.Draw(test);
    defaultScene.Draw(playerGun);
    for(int b = 0; b < bulletList.size(); b++){
      defaultScene.Draw(bulletList[b]);
      bulletList[b].move(bulletMove[b]);
      if (!defaultCam.isInCamView(bulletList[b])){
        bulletList.erase(bulletList.begin()+b);
        bulletMove.erase(bulletMove.begin()+b);
      }
    }

        //mouvement du joueur
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      player.move(Vector2f(0.0f,-10.0f));
      spriteAnimState++;
      if(box.getCollisionBox()->check(player.getCollisionBox()))
        player.move(Vector2f(0.0f,10.0f));
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
      player.move(Vector2f(0.0f,10.0f));
      spriteAnimState++;
      if(box.getCollisionBox()->check(player.getCollisionBox()))
        player.move(Vector2f(0.0f,-10.0f));
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
      player.move(Vector2f(10.0f,0.0f));
      spriteAnimState++;
      if(box.getCollisionBox()->check(player.getCollisionBox()))
        player.move(Vector2f(-10.0f,0.0f));
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
      player.move(Vector2f(-10.0f,0.0f));
      spriteAnimState++;
      if(box.getCollisionBox()->check(player.getCollisionBox()))
        player.move(Vector2f(10.0f,0.0f));
    }
    defaultCam.setPosition(player.getPosition());

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
      if(glfwGetTime()-timefromReload>1.5){
        timefromReload = glfwGetTime();
        SoundEngine->play2D("sound/gun-reload.wav",false);
      }
    }

    playerGun.setPosition(player.getPosition());
    test.setPosition(player.getPosition());

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    Vector2f mouse(defaultCam.screenPosToCamPos(Vector2f(mouseX,mouseY)));
    Vector2f disctanceMouseGun(mouse.x-player.getPosition().x,mouse.y-player.getPosition().y);

    float rotationAngle = std::atan(disctanceMouseGun.y/disctanceMouseGun.x);
    if(disctanceMouseGun.x < 0)
      rotationAngle -= M_PI;

    player.setRotation(rotationAngle);
    playerGun.setRotation(player.getRotation());
    test.setRotation(player.getRotation()-M_PI);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
      if (glfwGetTime()-timefromLastShoot>0.2){
        timefromLastShoot = glfwGetTime();
        spriteAnimStateGun++;
        SoundEngine->play2D("sound/gun-shoot_0.wav", false);
        Vector2f delta(mouse.x-playerGun.getPosition().x,mouse.y-player.getPosition().y);
        float lenght = sqrt(pow(delta.x,2)+pow(delta.y,2));
        Vector2f unitVec(delta.x/lenght,delta.y/lenght);
        Vector2f speed(unitVec.x*bulletSpeed.x,unitVec.y*bulletSpeed.y);
        RectangleShape bullet;
        bullet.setSize(Vector2f(5.0f,3.0f));
        bullet.setColor(Vector3i(255,0,0));
        bullet.setOrigin(Vector2f(-playerGun.getOrigin().x,-playerGun.getOrigin().y+17.5f));
        bullet.setPosition(Vector2f(playerGun.getPosition().x,playerGun.getPosition().y));
        bullet.setRotation(playerGun.getRotation());
        bulletMove.push_back(speed);
        bulletList.push_back(bullet);
      }
    }
    //////////////////////////////////////////////////////////////
    
    bbopErrorCheck();

    // Swap the back buffer with the front buffer
	  glfwSwapBuffers(window);
	  glfwPollEvents();
	}
	// Delete window before ending the program
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
