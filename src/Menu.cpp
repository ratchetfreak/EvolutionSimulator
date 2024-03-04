#include "../inc/Menu.hpp"
agl::Event		 *MenuShare::event;
agl::Texture	 *MenuShare::blank;
void			 *MenuShare::focusedMenu;
bool			 *MenuShare::leftClick;
FocusableElement *FocusableElement::focusedField;
agl::Shader		 *MenuShare::menuShader;
agl::Shader		 *MenuShare::baseShader;
agl::Camera		 *MenuShare::camera;

std::unique_ptr<agl::Text	  >    MenuShare::text;
std::unique_ptr<agl::Text	  >    MenuShare::smallText;
std::unique_ptr<agl::Rectangle > MenuShare::rect;
std::unique_ptr<agl::Circle	  >  MenuShare::circ;
std::unique_ptr<agl::Texture	>  MenuShare::border;

