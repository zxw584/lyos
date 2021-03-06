import os
import configparser

ROOT_DIR = os.getcwd()
PREFIX = os.sep.join([ROOT_DIR, 'local'])
PREFIX_BIN = os.sep.join([PREFIX, 'bin'])
CROSSPREFIX = '/usr'
DESTDIR = os.popen('readlink -f ' + ROOT_DIR + '/../obj/destdir.' +
                   os.environ["ARCH"] + '/').read()[:-1]
SYSROOT = DESTDIR
try:
    TARGET = os.environ["SUBARCH"] + '-elf-lyos'
except:
    TARGET = 'i686-elf-lyos'

os.environ["PKG_CONFIG_LIBDIR"] = SYSROOT + '/usr/lib/pkgconfig'
os.environ["PKG_CONFIG_SYSROOT_DIR"] = SYSROOT
os.environ["TOOLCHAIN"] = SYSROOT + '/usr'

packs = configparser.ConfigParser()
packs.read('packages.list')

GCC_VERSION = 'gcc-' + packs.get('gcc', 'version')
GCC_REPO = packs.get('gcc', 'repo')
GCC_TARBALL = GCC_VERSION + '.tar.gz'
GCC_TARBALL_URL = GCC_REPO + GCC_VERSION + '/' + GCC_TARBALL

BINUTILS_VERSION = 'binutils-' + packs.get('binutils', 'version')
BINUTILS_REPO = packs.get('binutils', 'repo')
BINUTILS_TARBALL = BINUTILS_VERSION + '.tar.bz2'
BINUTILS_TARBALL_URL = BINUTILS_REPO + BINUTILS_TARBALL

NEWLIB_VERSION = 'newlib-' + packs.get('newlib', 'version')
NEWLIB_REPO = packs.get('newlib', 'repo')
NEWLIB_TARBALL = NEWLIB_VERSION + '.tar.gz'
NEWLIB_TARBALL_URL = NEWLIB_REPO + NEWLIB_TARBALL

COREUTILS_VERSION = 'coreutils-' + packs.get('coreutils', 'version')
COREUTILS_REPO = packs.get('coreutils', 'repo')
COREUTILS_TARBALL = COREUTILS_VERSION + '.tar.gz'
COREUTILS_TARBALL_URL = COREUTILS_REPO + COREUTILS_TARBALL

DASH_VERSION = 'dash-' + packs.get('dash', 'version')
DASH_REPO = packs.get('dash', 'repo')
DASH_TARBALL = DASH_VERSION + '.tar.gz'
DASH_TARBALL_URL = DASH_REPO + DASH_TARBALL

NANO_VERSION = 'nano-' + packs.get('nano', 'version')
NANO_REPO = packs.get('nano', 'repo')
NANO_TARBALL = NANO_VERSION + '.tar.gz'
NANO_TARBALL_URL = NANO_REPO + NANO_TARBALL

NCURSES_VERSION = 'ncurses-' + packs.get('ncurses', 'version')
NCURSES_REPO = packs.get('ncurses', 'repo')
NCURSES_TARBALL = NCURSES_VERSION + '.tar.gz'
NCURSES_TARBALL_URL = NCURSES_REPO + NCURSES_TARBALL

BASH_VERSION = 'bash-' + packs.get('bash', 'version')
BASH_REPO = packs.get('bash', 'repo')
BASH_TARBALL = BASH_VERSION + '.tar.gz'
BASH_TARBALL_URL = BASH_REPO + BASH_TARBALL
