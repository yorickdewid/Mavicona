<?php

namespace Ecoli;

class Database implements SingletonContract
{
	/**
	 * Core application object.
	 *
	 * @var Ecoli\Application
	 */
	static $app;

	/**
	 * PDO object.
	 *
	 * @var PDO
	 */
	private $db = null;

	/**
	 * @var Singleton The reference to *Singleton* instance of this class
	 */
	private static $instance;

	/**
	 * Read from config file if exist.
	 *
	 * @return void
	 */
	public function getdb()
	{
		if (is_null($this->db)) {
			$this->db = new \PDO(
				$this->app->config['database']['type'] . ':host=' . $this->app->config['database']['host'] . ';dbname=' . $this->app->config['database']['name'],
				$this->app->config['database']['username'],
				$this->app->config['database']['password']
			);
		}

		return $this->db;
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
	public static function query($sql)
	{
		$object = static::getInstance();

		$rs = $object->getdb()->query($sql);

		return $rs->fetchAll(\PDO::FETCH_ASSOC);
	}

	/**
	 * Register any hook and run initializer routines.
	 *
	 * @return void
	 */
	public function boot()
	{

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