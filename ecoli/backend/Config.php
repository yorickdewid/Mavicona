<?php

namespace Ecoli;

class Config implements SingletonContract
{
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
		if (file_exists(self::$defaultConfig)) {
			$config = parse_ini_file(self::$defaultConfig);
		}
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
	 * Return current config.
	 *
	 * @return Array
	 */
	public function getConfig()
	{
		return $this->config;
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