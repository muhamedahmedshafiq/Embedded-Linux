ITI_GPIO_VERSION = 1.0
ITI_GPIO_SITE = $(TOPDIR)/package/iti-gpio
ITI_GPIO_SITE_METHOD = local
ITI_GPIO_INSTALL_TARGET = YES
ITI_GPIO_DEPENDENCIES = qt6base qt6declarative qt6shadertools

ITI_GPIO_CONF_OPTS = \
    -DCMAKE_PREFIX_PATH=$(STAGING_DIR)/usr/lib/cmake

$(eval $(cmake-package))
