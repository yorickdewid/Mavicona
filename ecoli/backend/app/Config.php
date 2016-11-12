<?php

namespace Ecoli;

class Config implements SingletonContract
{
	/**
	 * Core application object.
	 *
	 * @var Ecoli\Application
	 */
	static $app;

	/**
	 * Default config file.
	 *
	 * @var string
	 */
	static $defaultConfig = ".conf";

	/**
	 * INI config settings.
	 *
	 * @var Array
	 */
	private $config = [];

	/**
	 * @var Singleton The reference to *Singleton* instance of this class
	 */
	private static $instance;

	/**
	 * Read from config file if exist.
	 *
	 * @return void
	 */
	private function readConfig()
	{
		$dir = __DIR__ . '/../';
		if (!file_exists($dir . self::$defaultConfig)) {
			throw new CoreException('No config file found');
		}
		$this->config = parse_ini_file($dir . self::$defaultConfig);
	}

	/**
	 * Return current config.
	 *
	 * @return Array
	 */
	public function getConfigOption($key)
	{
		return $this->config[$key];
	}

	/**
	 * Returns config option.
	 *
	 * @return mixed.
	 */
	public static function option($key)
	{
		$object = static::getInstance();
		return $object->getConfigOption($key);
	}

	/**
	 * Returns the *Singleton* instance of this class.
	 *
	 * @return Singleton The *Singleton* instance.
	 */
	public static function getInstance()
	{
		if (null === static::$instance) {
			static::$instance = new static();
		}
		
		return static::$instance;
	}

	/**
	 * Register any hook and run initializer routines.
	 *
	 * @return void
	 */
	public function boot()
	{
		$this->readConfig();
	}

	/**
	 * Inject core application.
	 *
	 * @return void
	 */
	public function inject($core)
	{
		$this->app = $core;
	}

	/**
	 * Protected constructor to prevent creating a new instance of the
	 * *Singleton* via the `new` operator from outside of this class.
	 */
	protected function __construct()
	{
		
	}

	/**
	 * Private clone method to prevent cloning of the instance of the
	 * *Singleton* instance.
	 *
	 * @return void
	 */
	private function __clone()
	{

	}

	/**
	 * Private unserialize method to prevent unserializing of the *Singleton*
	 * instance.
	 *
	 * @return void
	 */
	private function __wakeup()
	{

	}

}
